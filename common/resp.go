package common

import ()

type PushResponse struct {
	Code string `json:"code"`
	Msg  string `json:"msg"`
}

type LogResponse struct {
	Issued uint64 `json:"issued"`
	Total  uint64 `json:"total"`
	Logs   string `json:"logs"`
}
