package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/jinzhu/gorm"
	"net/http"
	"time"
)

type TaskSet struct {
	Name     string
	worker   *Worker
	client   *http.Client
	parser   ParserHandler
	sleep    time.Duration
	fetcher  FetcherHandler
	database *gorm.DB
}

func (taskset *TaskSet) Sleep(d time.Duration) *TaskSet {
	taskset.sleep = d
	return taskset
}

func (taskset *TaskSet) Client(name string) *TaskSet {
	taskset.client = taskset.worker.GetClient(name)
	return taskset
}

func (taskset *TaskSet) Fetcher(f FetcherHandler) *TaskSet {
	taskset.fetcher = f
	return taskset
}

func (taskset *TaskSet) Parser(p ParserHandler) *TaskSet {
	taskset.parser = p
	return taskset
}

func (taskset *TaskSet) Database(name string) *TaskSet {
	taskset.database = taskset.worker.GetDB(name)
	return taskset
}

func (taskset *TaskSet) Wait() {
	if taskset.sleep != 0 {
		time.Sleep(time.Second * taskset.sleep)
	}
}

func (taskset *TaskSet) Parse(task *common.Task, bytes []byte) {
	taskset.parser(taskset, task, bytes)
}

func (taskset *TaskSet) Fetch(task *common.Task) []byte {
	return taskset.fetcher(taskset, task)
}

func (taskset *TaskSet) GetDB() *gorm.DB {
	return taskset.database
}

func (taskset *TaskSet) GetWorker() *Worker {
	return taskset.worker
}

func NewTaskSet(name string, worker *Worker) *TaskSet {
	taskset := new(TaskSet)
	taskset.Name = name
	taskset.worker = worker
	taskset.client = &http.Client{}
	taskset.fetcher = CommonFetcher
	return taskset
}
