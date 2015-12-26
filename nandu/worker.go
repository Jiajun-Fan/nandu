package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"io/ioutil"
	"net/http"
	"time"
)

const (
	kServerConfigFile = "server_c.json"
)

type Worker struct {
	info        *common.ServerInfo
	project     string
	retry_count uint
	retry_max   uint
	clients     map[string]*http.Client
	parsers     map[string]ParserHandler
}

func (worker *Worker) Push(task *common.Task) *common.Task {
	r, err := util.HttpPostJSON(worker.info.AddrPush(), task)
	if err != nil {
		util.Debug().Error("failed to push task, %s\n", err.Error())
		return nil
	}

	resp := new(common.CommonResponse)
	err = util.HttpResponseUnmarshalJSON(resp, r, http.StatusOK)

	if err != nil {
		util.Debug().Error("failed to push task, %s\n", err.Error())
		return nil
	} else {
		if resp.Task != nil {
			util.Debug().Debug(resp.Task.PushLog())
		}
	}
	return resp.Task
}

func (worker *Worker) Pop() *common.Task {
	r, err := util.HttpPostJSON(worker.info.AddrPop(), &common.Worker{worker.project})
	if err != nil {
		util.Debug().Error("failed to pop task, %s\n", err.Error())
		return nil
	}

	resp := new(common.CommonResponse)
	err = util.HttpResponseUnmarshalJSON(resp, r, http.StatusOK)

	if err != nil {
		util.Debug().Error("failed to pop task, %s\n", err.Error())
		return nil
	} else {
		if resp.Task != nil {
			util.Debug().Debug(resp.Task.PopLog())
		}
	}

	return resp.Task
}

func (worker *Worker) RegisterClient(taskset string, client *http.Client) {
	if _, ok := worker.clients[taskset]; !ok {
		worker.clients[taskset] = client
	} else {
		util.Debug().Warning("double register client for taskset %s\n", taskset)
	}
}

func (worker *Worker) RegisterParser(taskset string, parser ParserHandler) {
	if _, ok := worker.parsers[taskset]; !ok {
		worker.parsers[taskset] = parser
	} else {
		util.Debug().Warning("double register parser for taskset %s\n", taskset)
	}
}

func (worker *Worker) Run() {
	util.Debug().Info("'%s' started\n", worker.project)

	for {
		task := worker.Pop()
		if task != nil {
			data := worker.Fetch(task)
			if data != nil {
				worker.Parse(task, data)
			}
		} else {
			worker.retry_count += 1
			if worker.retry_count >= worker.retry_max {
				break
			}
			util.Debug().Info("sleep 1 second, ( %d | %d )\n", worker.retry_count, worker.retry_max)
			time.Sleep(time.Second)
		}
	}

	util.Debug().Info("'%s' exit\n", worker.project)
}

func (worker *Worker) Parse(task *common.Task, bytes []byte) {
	parser, ok := worker.parsers[task.TaskSet]
	if !ok {
		util.Debug().Error("Can't find parser for taskset %s\n", task.TaskSet)
		return
	}

	parser(worker, task, bytes)
}

func (worker *Worker) Fetch(task *common.Task) []byte {
	client, ok := worker.clients[task.TaskSet]
	if !ok {
		client = &http.Client{}
	}

	r, err := client.Get(task.Url)
	if err != nil {
		util.Debug().Error("can't fetch url %s, %s\n", task.Url, err.Error())
		return nil
	}

	defer r.Body.Close()

	bytes, err := ioutil.ReadAll(r.Body)

	if err != nil {
		util.Debug().Error("can't get bytes from url %s, %s\n", task.Url, err.Error())
		return nil
	}

	if r.StatusCode/100 != 2 {
		util.Debug().Error("get wrong status code url %d, %d\n", task.Url, r.StatusCode)
		return nil
	}

	return bytes
}

func NewWorker(project string) *Worker {
	worker := new(Worker)
	worker.project = project
	worker.retry_count = 0
	worker.clients = make(map[string]*http.Client)
	worker.parsers = make(map[string]ParserHandler)
	worker.retry_max = 10
	info, err := common.NewServerInfo(kServerConfigFile)
	if err != nil {
		util.Debug().Fatal("can't open config file %s\n", kServerConfigFile)
	}
	worker.info = info
	return worker
}
