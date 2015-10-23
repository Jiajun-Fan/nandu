package nandu

import ()

type Worker interface {
	Run()
	setName(name string)
	getName() string
	started() bool
}

type CoWorker struct {
	q        *Q
	started_ bool
	name     string
}

func (worker *CoWorker) started() bool {
	return worker.started_
}

func (worker *CoWorker) setName(name string) {
	worker.name = name
}

func (worker *CoWorker) getName() string {
	return worker.name
}

func (worker *CoWorker) Run() {
	DebugV(DebugInfo).Printf("'%s' started\n", worker.name)
	worker.started_ = true
	for {
		task := worker.q.Pop(worker)
		if task == nil {
			worker.started_ = false
			break
		}
		ret := task.Run(worker.q)
		for i := range ret {
			worker.q.Push(ret[i])
		}
	}
	worker.q.workerCount.Done()
	DebugV(DebugInfo).Printf("'%s' exit\n", worker.name)
}

func NewCoWorker(q *Q) Worker {
	worker := new(CoWorker)
	worker.q = q
	worker.started_ = false
	return worker
}
