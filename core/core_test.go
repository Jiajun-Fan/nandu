package core

import (
	"testing"
)

type MyTask struct {
	TaskBase
}

func TestQueueIsEmpty(t *testing.T) {
	q := NewQ()
	if err := q.RunOneTask(); err != ErrorQueueNoPendingTask {
		t.Error("Expected ErrorQueueNoPendingTask, got", err)
	}
}

func TestQueueRunTask(t *testing.T) {
	q := NewQ()
	task := MyTask{TaskBase{0, "ok"}}
	q.AddTask(&task)
	if err := q.RunOneTask(); err != nil {
		t.Error(err.Error())
	}
}
