package beigui

import ()

type Task struct {
	Id      uint64 `json:"id"`
	Project string `json:"project"`
	Url     string `json:"url"`
}
