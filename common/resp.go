package common

import ()

const (
	ResponseOK     = 0
	ResponseNoTask = 1
	ResponseFull   = 2
)

type CommonResponse struct {
	Code int   `json:"code"`
	Task *Task `json:"task,omitempty"`
}
