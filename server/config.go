package server

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
)

const kServerConfigFile = "server.json"

type ServerInfo struct {
	Addr  string `json:"addr"`
	Port  uint   `json:"port"`
	Token string `json:"token"`
}

func (s *ServerInfo) address() string {
	return fmt.Sprintf("%s:%d", s.Addr, s.Port)
}

func genServerInfoTemplate() {
	info := ServerInfo{
		"",
		0,
		"",
	}
	bytes, _ := json.Marshal(info)
	fn := fmt.Sprintf("%s.template", kServerConfigFile)
	ioutil.WriteFile(fn, bytes, 0644)
}

func newServerInfo() (*ServerInfo, error) {

	bytes, err := ioutil.ReadFile(kServerConfigFile)
	if err != nil {
		return nil, err
	}

	info := new(ServerInfo)
	err = json.Unmarshal(bytes, info)
	if err != nil {
		return nil, err
	} else {
		return info, err
	}
}
