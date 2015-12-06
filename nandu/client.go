package nandu

import (
	"errors"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"net/http"
)

const (
	kServerConfigFile = "server_c.json"
)

type Worker struct {
	info  *common.ServerInfo
	name  string
	count uint
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

func (worker *Worker) Push(task *common.Task) (*common.Task, error) {
	resp, err := responseToJSON(util.HttpPostJSON(worker.info.AddrPush(), task))
	if err != nil {
		util.Debug().Error("failed to push task %v\n", err)
		return nil, err
	}
	util.Debug().Info("%s\n", resp.Task)
	return resp.Task, err
}

func (worker *Worker) Pop() (*common.Task, error) {
	resp, err := responseToJSON(util.HttpPostJSON(worker.info.AddrPop(), &common.Worker{"cool"}))
	if err != nil {
		util.Debug().Error("failed to push task %v\n", err)
		return nil, err
	}
	util.Debug().Info("%s\n", resp.Task)
	return resp.Task, err
}

func (worker *Worker) Run() {
	util.Debug().Info("'%s' started\n", worker.name)

	util.Debug().Info("'%s' exit\n", worker.name)
}

func NewWorker(name string) *Worker {
	worker := new(Worker)
	worker.name = name
	worker.count = 0
	info, err := common.NewServerInfo(kServerConfigFile)
	if err != nil {
		util.Debug().Fatal("can't open config file %s\n", kServerConfigFile)
	}
	worker.info = info
	return worker
}
