package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
)

type DatabaseInfo struct {
	Name       string `json:"name"`
	DbType     string `json:"db_type"`
	ConnectStr string `json:"connect_string"`
	Init       bool   `json:"init"`
}

type OauthInfo struct {
	Name      string `json:"name"`
	AppKey    string `json:"app_key"`
	AppSecret string `json:"app_secret"`
	Token     string `json:"token"`
	Secret    string `json:"secret"`
}

type NanduInfo struct {
	Host      string            `json:"host"`
	Project   string            `json:"project"`
	Server    common.ServerInfo `json:"server"`
	Databases []DatabaseInfo    `json:"databases"`
	Oauths    []OauthInfo       `json:"oauths"`
}

func NewNanduInfo(file string) (*NanduInfo, error) {
	info := new(NanduInfo)
	err := util.ReadJsonFile(file, info)
	if err != nil {
		return nil, err
	}
	return info, err
}
