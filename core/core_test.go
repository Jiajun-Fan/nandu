package core

import (
	"testing"
)

type MyTask struct {
	TaskID
}

func (task MyTask) Run() error {
	return nil
}

func TestQueueIsEmpty(t *testing.T) {
	q := NewQ()
	if _, err := q.Pop(); err != ErrorQueueEmpty {
		t.Error("Expected ErrorQueueEmpty, got", err)
	}
}

func TestQueueSetTaskID(t *testing.T) {
	q := NewQ()
	task := MyTask{0}
	q.Push(&task)
	if taskPopped, err := q.Pop(); err != nil {
		t.Error(err.Error())
	} else {
		if id := taskPopped.GetID(); id != 1 {
			t.Error("Expected task ID is 1, got", id)
		}
	}
}
