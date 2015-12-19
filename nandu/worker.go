package nandu

import (
	"errors"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"net/http"
	"time"
)

const (
	kServerConfigFile = "server_c.json"
)

type Worker struct {
	info        *common.ServerInfo
	name        string
	retry_count uint
	retry_max   uint
	clients     map[string]Client
}

func responseToJSON(resp *http.Response, err error) (*common.CommonResponse, error) {
	if err != nil {
		return nil, err
	}
	if err == nil && (resp.StatusCode/100 != 2) {
		return nil, errors.New(resp.Status)
	}
	common_resp := new(common.CommonResponse)
	err = util.HttpResponseUnmarshalJSON(common_resp, resp)
	return common_resp, err
}

func (worker *Worker) Push(task *common.Task) *common.Task {
	resp, err := responseToJSON(util.HttpPostJSON(worker.info.AddrPush(), task))
	util.Debug().Info(resp.Task.PushLog())
	if err != nil {
		util.Debug().Error("failed to push task %v\n", err)
		return nil
	}
	return resp.Task
}

func (worker *Worker) Pop() *common.Task {
	resp, err := responseToJSON(util.HttpPostJSON(worker.info.AddrPop(), &common.Worker{"cool"}))
	if err != nil {
		util.Debug().Error("failed to pop task reason %v\n", err)
		return nil
	} else {
		util.Debug().Info(resp.Task.PopLog())
	}
	return resp.Task
}

func (worker *Worker) RegisterClient(client Client) {
	_, ok := worker.clients[client.Name()]
	if !ok {
		worker.clients[client.Name()] = client
	} else {
		util.Debug().Warning("double register client %s\n", client.Name())
	}
}

func (worker *Worker) Run() {
	util.Debug().Info("'%s' started\n", worker.name)

	for {
		task := worker.Pop()
		if task != nil {
			if client, ok := worker.clients[task.TaskSet]; ok {
				client.Run(task)
			} else {
				util.Debug().Error("can't file client for taskset %s\n", task.TaskSet)
			}
		} else {
			worker.retry_count += 1
			if worker.retry_count >= worker.retry_max {
				break
			}
			util.Debug().Info("sleep 1 second, ( %d | %d )", worker.retry_count, worker.retry_max)
			time.Sleep(time.Second)
		}
	}

	util.Debug().Info("'%s' exit\n", worker.name)
}

func NewWorker(name string) *Worker {
	worker := new(Worker)
	worker.name = name
	worker.retry_count = 0
	info, err := common.NewServerInfo(kServerConfigFile)
	if err != nil {
		util.Debug().Fatal("can't open config file %s\n", kServerConfigFile)
	}
	worker.info = info
	return worker
}
