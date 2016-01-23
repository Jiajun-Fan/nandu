package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"io/ioutil"
	"net/http"
)

type TaskSet struct {
	Name   string
	worker *Worker
	client *http.Client
	parser ParserHandler
}

func (taskset *TaskSet) Client(name string) *TaskSet {
	taskset.client = taskset.worker.GetClient(name)
	return taskset
}

func (taskset *TaskSet) Parser(p ParserHandler) *TaskSet {
	taskset.parser = p
	return taskset
}

func (taskset *TaskSet) Fetch(task *common.Task) []byte {
	r, err := taskset.client.Get(task.Url)
	if err != nil {
		util.Error("can't fetch url %s, %s\n", task.Url, err.Error())
		return nil
	}

	defer r.Body.Close()

	bytes, err := ioutil.ReadAll(r.Body)

	if err != nil {
		util.Error("can't get bytes from url %s, %s\n", task.Url, err.Error())
		return nil
	}

	if r.StatusCode/100 != 2 {
		util.Error("get wrong status code url %s, %d\n", task.Url, r.StatusCode)
		return nil
	}

	return bytes
}

func (taskset *TaskSet) Parse(task *common.Task, bytes []byte) {
	taskset.parser(taskset.worker, task, bytes)
}
