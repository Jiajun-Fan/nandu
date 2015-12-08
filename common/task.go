package common

import (
	"fmt"
)

type Task struct {
	Id      uint64 `json:"id"`
	Project string `json:"project"`
	TaskSet string `json:"task_set"`
	Url     string `json:"url"`
	Data    string `json:"data"`
}

func (t *Task) PushLog() string {
	return fmt.Sprintf("[ PUSH ]: project: %s, task_set: %s, url: %s, data: %s\n", t.Project, t.TaskSet, t.Url, t.Data)
}

func (t *Task) PopLog() string {
	return fmt.Sprintf("[ POP  ]: id: %d, project: %s, task_set: %s, url: %s, data: %s\n", t.Id, t.Project, t.TaskSet, t.Url, t.Data)
}
