package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"io/ioutil"
	"net/http"
	"time"
)

const (
	kNanduConfigFile = "nandu.json"
)

type TaskSet struct {
	Name   string `json:"name"`
	client *http.Client
	parser ParserHandler
}

type Worker struct {
	info        *NanduInfo
	project     string
	retry_count uint
	retry_max   uint
	clients     map[string]*http.Client
	tasksets    map[string]*TaskSet
	database    Database
}

func (worker *Worker) GetDB() Database {
	if worker.database == nil {
		util.Debug().Fatal("no database availabel\n")
	}
	return worker.database
}

func (worker *Worker) Push(task *common.Task) *common.Task {
	task.Token = worker.info.Server.Token
	r, err := util.HttpPostJSON(worker.info.Server.AddrPush(), task)
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
	r, err := util.HttpPostJSON(worker.info.Server.AddrPop(), &common.Worker{worker.info.Server.Token, worker.project})
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

func (worker *Worker) registerDatabase() {
	if worker.info.Database.DbType == "" || worker.info.Database.ConnectStr == "" {
		return
	}
	if database, err := NewDatabase(worker.info.Database.DbType, worker.info.Database.ConnectStr); err != nil {
		util.Debug().Error("can't connect to database %s\n", err.Error())
	} else {
		worker.database = database
	}
}

func (worker *Worker) registerClients() {
	worker.clients = make(map[string]*http.Client)
	for i := range worker.info.Oauths {
		oinfo := worker.info.Oauths[i]
		oauth := NewOauth(oinfo.AppKey, oinfo.AppSecret, oinfo.Token, oinfo.Secret)
		if oauth == nil {
			continue
		}

		if _, ok := worker.clients[oinfo.Name]; !ok {
			worker.clients[oinfo.Name] = oauth
		} else {
			util.Debug().Warning("double register client for oauth %s\n", oinfo.Name)
		}
	}
}

func (worker *Worker) registerTaskSets() {
	worker.tasksets = make(map[string]*TaskSet)
	for i := range worker.info.TaskSets {
		tinfo := worker.info.TaskSets[i]
		taskset := new(TaskSet)
		taskset.Name = tinfo.Name
		if tinfo.Client == "" {
			taskset.client = &http.Client{}
		} else {
			if client, ok := worker.clients[tinfo.Client]; ok {
				taskset.client = client
			} else {
				util.Debug().Warning("can't find client named %s\n", tinfo.Client)
				taskset.client = &http.Client{}
			}
		}

		if _, ok := worker.tasksets[taskset.Name]; !ok {
			worker.tasksets[taskset.Name] = taskset
		} else {
			util.Debug().Warning("double register taskset named %s\n", taskset.Name)
		}
	}
}

func (worker *Worker) RegisterParser(name string, parser ParserHandler) {
	if taskset, ok := worker.tasksets[name]; ok {
		if taskset.parser == nil {
			taskset.parser = parser
		} else {
			util.Debug().Warning("double register parser for taskset %s\n", name)
		}
	} else {
		util.Debug().Warning("can't find taskset %s\n", name)
	}
}

func (worker *Worker) checkParsers() {
	for i := range worker.tasksets {
		if worker.tasksets[i].parser == nil {
			util.Debug().Fatal("missing parser of tasksets %s\n", worker.tasksets[i].Name)
		}
	}
}

func (worker *Worker) pushInitTasks() {
	for i := range worker.info.InitTasks {
		task := &worker.info.InitTasks[i]
		worker.Push(task)
	}
}

func (worker *Worker) Run() {
	worker.checkParsers()
	worker.pushInitTasks()

	util.Debug().Info("'%s' started\n", worker.project)

	for {
		task := worker.Pop()
		if task != nil {
			if _, ok := worker.tasksets[task.TaskSet]; !ok {
				util.Debug().Error("can't find taskset %s\n", task.TaskSet)
				continue
			}
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
	parser := worker.tasksets[task.TaskSet].parser
	parser(worker, task, bytes)
}

func (worker *Worker) Fetch(task *common.Task) []byte {
	client := worker.tasksets[task.TaskSet].client

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

func NewWorker() *Worker {
	worker := new(Worker)
	worker.retry_count = 0
	worker.retry_max = 10
	info, err := NewNanduInfo(kNanduConfigFile)
	if err != nil {
		util.Debug().Fatal("failed to load config file %s\n", err.Error())
	}
	worker.info = info
	worker.project = info.Project
	worker.registerClients()
	worker.registerTaskSets()
	worker.registerDatabase()
	return worker
}
