package beigui

import (
	"container/list"
	"sync"
)

type Q struct {
	cnt_issued uint64
	cnt_total  uint64
	list       list.List
	lock       sync.Mutex
}

func (q *Q) Push(data Task) {
	q.lock.Lock()
	q.cnt_total += 1
	q.list.PushBack(data)
	q.lock.Unlock()
}

func (q *Q) Pop() Task {
	q.lock.Lock()
	q.cnt_issued += 1
	data := q.list.Remove(q.list.Front())
	q.lock.Unlock()
	task, _ := data.(Task)
	return task
}

func (q *Q) Issued() uint64 {
	return q.cnt_issued
}

func (q *Q) Total() uint64 {
	return q.cnt_total
}

func MakeQ() *Q {
	q := new(Q)
	q.cnt_issued = 0
	q.cnt_total = 0
	return q
}
