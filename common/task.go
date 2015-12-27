package common

import (
	"errors"
	"fmt"
)

type Task struct {
	Id      uint64 `json:"id"`
	Project string `json:"project"`
	TaskSet string `json:"task_set"`
	Url     string `json:"url"`
	Token   string `json:"token"`
	Data    struct {
		Pint []int64  `json:"pint"`
		Pstr []string `json:"pstring"`
	} `json:"data"`
}

func (t *Task) AppendStrData(d string) {
	t.Data.Pstr = append(t.Data.Pstr, d)
}

func (t *Task) GetStrData(i int) (string, error) {
	if len(t.Data.Pstr) <= i {
		return "", errors.New("string data index overflow")
	}
	return t.Data.Pstr[i], nil
}

func (t *Task) AppendIntData(i int64) {
	t.Data.Pint = append(t.Data.Pint, i)
}

func (t *Task) GetIntData(i int) (int64, error) {
	if len(t.Data.Pint) <= i {
		return 0, errors.New("int data index overflow")
	}
	return t.Data.Pint[i], nil
}

func (t *Task) PushLog() string {
	return fmt.Sprintf("[ PUSH ]: project: %s, task_set: %s, url: %s, data: %+v\n", t.Project, t.TaskSet, t.Url, t.Data)
}

func (t *Task) PopLog() string {
	return fmt.Sprintf("[ POP  ]: project: %s, task_set: %s, url: %s, data: %+v\n", t.Project, t.TaskSet, t.Url, t.Data)
}
