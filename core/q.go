package core

import (
	"container/list"
	"errors"
	"sync"
)

var (
	ErrorQueueEmpty error = errors.New("queue is empty")
)

type Q struct {
	cntIssued uint64
	cntTotal  uint64
	list      list.List
	lock      sync.Mutex
}

func (q *Q) Push(t Task) error {
	q.lock.Lock()
	q.cntTotal += 1
	t.Init(q.cntTotal)
	q.list.PushBack(t)
	q.lock.Unlock()
	return nil
}

func (q *Q) Pop() (Task, error) {
	q.lock.Lock()

	defer q.lock.Unlock()

	if q.list.Len() > 0 {
		data := q.list.Remove(q.list.Front())
		if task, ok := data.(Task); ok {
			q.cntIssued += 1
			return task, nil
		} else {
			panic("Unknown data type pushed into q")
		}
	}
	return nil, ErrorQueueEmpty
}

func NewQ() *Q {
	q := new(Q)
	q.cntIssued = 0
	q.cntTotal = 0
	return q
}
