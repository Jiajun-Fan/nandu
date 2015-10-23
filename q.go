package nandu

import (
	"container/list"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"sync"
	"time"
)

type Q struct {
	list list.List
	lock sync.Mutex

	workers      list.List
	workerCount  sync.WaitGroup
	workerNumber uint32

	last_push int64
	timeout   int64
}

func (q *Q) Push(data Task) {
	q.lock.Lock()
	q.list.PushBack(data)
	q.last_push = time.Now().Unix()
	q.lock.Unlock()
}

func (q *Q) Pop(w Worker) Task {
	for {
		q.lock.Lock()
		size := q.list.Len()
		if size == 0 {
			q.lock.Unlock()
			if remain := time.Now().Unix() - q.timeout - q.last_push; remain > 0 {
				return nil
			} else {
				DebugV(DebugDebug).Printf("%s sleep 1 second, %d second(s) remains!\n", w.getName(), remain*-1)
				time.Sleep(time.Second)
			}
		} else {
			data := q.list.Remove(q.list.Front())
			q.lock.Unlock()
			task, _ := data.(Task)
			return task
		}
	}
}

func (q *Q) AddWorker() {
	worker := NewCoWorker(q)
	q.workerNumber++
	name := fmt.Sprintf("CoWorker %d", q.workerNumber)
	worker.setName(name)
	q.workers.PushBack(worker)
	debug.V(3).Printf("Add worker '%s'\n", name)
}

func (q *Q) Run() {
	for it := q.workers.Front(); it != nil; it = it.Next() {
		worker := it.Value.(Worker)
		if !worker.started() {
			q.workerCount.Add(1)
			go worker.Run()
		}
	}
	q.workerCount.Wait()
}

func (q *Q) readTask() {
	defer Pass()

	bs, err := ioutil.ReadFile("nandu.json")
	AssertNil(err)

	jsonTasks := new(JsonTasks)
	err = json.Unmarshal(bs, jsonTasks)
	AssertNil(err)

	tasks := NewTasks(jsonTasks)
	for i := range tasks {
		q.Push(tasks[i])
	}
}

func MakeQ(timeout int64) *Q {
	q := new(Q)
	q.readTask()
	q.timeout = timeout
	q.last_push = time.Now().Unix()
	return q
}
