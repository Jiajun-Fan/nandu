package common

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
)

type ServerInfo struct {
	Addr  string `json:"addr"`
	Port  uint   `json:"port"`
	Token string `json:"token"`
}

func (s *ServerInfo) Address() string {
	return fmt.Sprintf("%s:%d", s.Addr, s.Port)
}

func (s *ServerInfo) AddrPush() string {
	return fmt.Sprintf("%s:%d/push", s.Addr, s.Port)
}

func (s *ServerInfo) AddrPop() string {
	return fmt.Sprintf("%s:%d/pop", s.Addr, s.Port)
}

func NewServerInfo(file string) (*ServerInfo, error) {

	bytes, err := ioutil.ReadFile(file)
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
