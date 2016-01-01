package nandu

import (
	"bufio"
	"crypto/md5"
	"fmt"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"os"
)

type DownloadData struct {
	FileName string `json:"filename"`
}

type FileData struct {
	ID       uint `gorm:"primary_key"`
	Host     string
	FileName string
	Hash     string `json:"-" sql:"unique;unique_index:file_md5"`
}

func DownloadParser(worker *Worker, task *common.Task, bytes []byte) {

	d := DownloadData{}
	task.GetData(&d)
	filename := d.FileName

	if filename == "" {
		util.Debug().Error("no file name specified\n")
		return
	}

	h := md5.New()
	md5 := fmt.Sprintf("%x", h.Sum(bytes))

	fileData := FileData{}
	fileData.Host = worker.info.Host
	fileData.FileName = filename
	fileData.Hash = md5

	old_file := FileData{}
	worker.GetDB().Where("hash = ?", fileData.Hash).First(&old_file)
	if !worker.GetDB().NewRecord(old_file) {
		return
	}

	if f, err := os.Create(filename); err != nil {
		util.Debug().Error("can't create file %s\n", err.Error())
	} else {
		defer f.Close()

		w := bufio.NewWriter(f)
		w.Write(bytes)
		w.Flush()

		util.Debug().Info("downloading %s\n", task.Url)
		worker.GetDB().Create(&fileData)
	}
}
