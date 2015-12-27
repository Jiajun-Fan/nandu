package nandu

import (
	"encoding/json"
	"github.com/Jiajun-Fan/nandu/common"
	"io/ioutil"
)

type DatabaseInfo struct {
	DbType     string `json:"db_type"`
	ConnectStr string `json:"connect_string"`
}

type OauthInfo struct {
	Name      string `json:"name"`
	AppKey    string `json:"app_key"`
	AppSecret string `json:"app_secret"`
	Token     string `json:"token"`
	Secret    string `json:"secret"`
}

type TaskSetInfo struct {
	Name   string `json:"name"`
	Client string `json:"client"`
}

type NanduInfo struct {
	Project   string            `json:"project"`
	Server    common.ServerInfo `json:"server"`
	Database  DatabaseInfo      `json:"database"`
	Oauths    []OauthInfo       `json:"oauths"`
	TaskSets  []TaskSetInfo     `json:"tasksets"`
	InitTasks []common.Task     `json:"init_tasks"`
}

func NewNanduInfo(file string) (*NanduInfo, error) {
	bytes, err := ioutil.ReadFile(file)
	if err != nil {
		return nil, err
	}

	info := new(NanduInfo)
	err = json.Unmarshal(bytes, info)
	if err != nil {
		return nil, err
	}
	return info, err
}
