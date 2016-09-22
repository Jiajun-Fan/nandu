package server

import (
	"container/list"
	"github.com/Jiajun-Fan/nandu/common"
	"sync"
)

type Q struct {
	cnt_issued uint64
	cnt_total  uint64
	list       list.List
	lock       sync.Mutex
}

func (q *Q) push(data *common.Task) {
	q.lock.Lock()
	q.cnt_total += 1
	q.list.PushBack(data)
	q.lock.Unlock()
}

func (q *Q) pop(w *common.Worker) *common.Task {
	q.lock.Lock()
	var task *common.Task = nil
	if q.list.Len() > 0 {
		q.cnt_issued += 1
		data := q.list.Remove(q.list.Front())
		task, _ = data.(*common.Task)
	}
	q.lock.Unlock()
	return task
}

func newQ() *Q {
	q := new(Q)
	q.cnt_issued = 0
	q.cnt_total = 0
	return q
}
