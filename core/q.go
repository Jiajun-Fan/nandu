package core

import (
	"container/list"
	"errors"
	"sync"
)

var (
	ErrorQueueNoPendingTask = errors.New("there is no pending task in the queue")
)

type Q struct {
	cntIssued uint64
	cntTotal  uint64
	list      list.List
	lock      sync.Mutex
}

func (q *Q) AddTask(t Task) error {
	q.lock.Lock()
	q.cntTotal += 1
	t.Init(q.cntTotal)
	q.list.PushBack(t)
	// TODO log new task
	q.lock.Unlock()
	return nil
}

func (q *Q) RunOneTask() error {
	q.lock.Lock()

	if q.list.Len() > 0 {
		data := q.list.Remove(q.list.Front())
		if task, ok := data.(Task); ok {
			q.cntIssued += 1
			q.lock.Unlock()
			if err := task.Run(); err == nil {
				// TODO log success
			} else {
				// TODO log failed
			}
			return nil
		} else {
			panic("Unknown data type pushed into q")
		}
	}
	q.lock.Unlock()
	return ErrorQueueNoPendingTask
}

func NewQ() *Q {
	q := new(Q)
	q.cntIssued = 0
	q.cntTotal = 0
	return q
}
