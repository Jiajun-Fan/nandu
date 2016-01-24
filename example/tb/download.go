package main

import (
	"bytes"
	"crypto/aes"
	"crypto/cipher"
	"crypto/md5"
	"crypto/rand"
	"errors"
	"fmt"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/nandu"
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/aliyun/aliyun-oss-go-sdk/oss"
	"github.com/jinzhu/gorm"
	"io"
)

const (
	kAesBlockSize     = 32
	kDownloadInfoFile = "download.json"
)

type DownloadInfo struct {
	AppKey     string      `json:"appkey"`
	AppSecret  string      `json:"appsecret"`
	BucketName string      `json:"bucket"`
	EndPoint   string      `json:"endpoint"`
	EncryptKey string      `json:"encrypt_key"`
	Bucket     *oss.Bucket `json:"-"`
}

type DownloadData struct {
	Pid uint `json:"pid"`
}

func NewDownloadInfo(file string) (*DownloadInfo, error) {
	info := new(DownloadInfo)
	err := util.ReadJsonFile(file, info)
	if err != nil {
		return nil, err
	}
	if info.AppKey == "" || info.AppSecret == "" || info.BucketName == "" || info.EndPoint == "" || info.EncryptKey == "" {
		return nil, errors.New("missing required download information")
	}

	client, err := oss.New(info.EndPoint, info.AppKey, info.AppSecret)
	if err != nil {
		return nil, errors.New("can't connect to aliyun oss")
	}

	bucket, err := client.Bucket(info.BucketName)
	if err != nil {
		return nil, errors.New("can't get specified bucket")
	}
	info.Bucket = bucket

	return info, err
}

var gDownloadInfo *DownloadInfo = nil

type FileData struct {
	ID       uint `gorm:"primary_key"`
	FileName string
	Hash     string `json:"-" sql:"unique;unique_index:file_md5"`
}

func paddingKey(key []byte) []byte {
	ret := make([]byte, kAesBlockSize)
	copy(ret, key)
	return ret
}

func paddingBytes(bytes []byte) []byte {
	length := (len(bytes) + aes.BlockSize - 1) / aes.BlockSize * aes.BlockSize
	ret := make([]byte, length)
	copy(ret, bytes)
	return ret
}

func encrypt(bytes []byte, k string) ([]byte, []byte) {
	key := []byte(k)

	block, err := aes.NewCipher(paddingKey(key))
	if err != nil {
		panic(err)
	}

	bytes = paddingBytes(bytes)
	buff := make([]byte, aes.BlockSize+len(bytes))
	iv := buff[:aes.BlockSize]
	if _, err := io.ReadFull(rand.Reader, iv); err != nil {
		panic(err)
	}

	mode := cipher.NewCBCEncrypter(block, iv)
	mode.CryptBlocks(buff[aes.BlockSize:], bytes)

	return iv, buff
}

func duplicate(db *gorm.DB, hash string) (bool, uint) {
	file := FileData{}
	db.Where("hash = ?", hash).First(&file)
	return !db.NewRecord(file), file.ID
}

func setFileId(db *gorm.DB, pid uint, fid uint) {
	photo := TumblrPhoto{}
	db.Where("id = ?", pid).First(&photo)
	if !db.NewRecord(photo) {
		photo.FileDataID = fid
		db.Save(&photo)
	}
}

func DownloadParser(taskset *nandu.TaskSet, task *common.Task, bs []byte) {

	if gDownloadInfo == nil {
		var err error
		gDownloadInfo, err = NewDownloadInfo(kDownloadInfoFile)
		if err != nil {
			util.Fatal("%s\n", err.Error())
		}
	}

	h := md5.New()
	h.Write(bs)
	hash := fmt.Sprintf("%x", h.Sum(nil))

	data := DownloadData{}
	task.GetData(&data)

	if dup, fid := duplicate(taskset.GetDB(), hash); dup {
		setFileId(taskset.GetDB(), data.Pid, fid)
		return
	}

	iv, bs := encrypt(bs, gDownloadInfo.EncryptKey)

	fileData := FileData{}
	fileData.FileName = fmt.Sprintf("%x%s", iv[:4], hash[:24])
	fileData.Hash = hash

	err := gDownloadInfo.Bucket.PutObject(fileData.FileName, bytes.NewReader(bs))
	if err != nil {
		util.Error("can't create file %s\n", err.Error())
	} else {
		util.Info("downloading %s\n", task.Url)
		taskset.GetDB().Create(&fileData)
		setFileId(taskset.GetDB(), data.Pid, fileData.ID)
	}
}
