package main

import (
	"fmt"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"net/http"
)

var g_info *common.ServerInfo = nil
var g_q *Q = nil
var g_log *util.Log = nil

const kServerConfigFile = "beigui.json"

func check_init() {
	if g_q == nil || g_log == nil || g_info == nil {
		panic("not initialized")
	}
}

func push(w http.ResponseWriter, r *http.Request) {
	check_init()

	task := new(common.Task)
	err := util.HttpRequestUnmarshalJSON(task, r)

	if task.Token == "" || task.Token != g_info.Token {
		w.WriteHeader(http.StatusBadRequest)
		util.HttpRespondText("missing token", w)
		return
	}

	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		util.HttpRespondText(err.Error(), w)
		return
	}

	if task.Project == "" {
		w.WriteHeader(http.StatusBadRequest)
		util.HttpRespondText("missing project name", w)
		return
	}

	if task.TaskSet == "" {
		w.WriteHeader(http.StatusBadRequest)
		util.HttpRespondText("missing taskset name", w)
		return
	}

	if task.Url == "" {
		w.WriteHeader(http.StatusBadRequest)
		util.HttpRespondText("missing url", w)
		return
	}

	g_q.Push(task)

	g_log.Write(task.PushLog())

	util.HttpRespondMarshalJSON(&common.CommonResponse{common.ResponseOK, task}, w)
}

func pop(w http.ResponseWriter, r *http.Request) {
	check_init()

	worker := new(common.Worker)
	err := util.HttpRequestUnmarshalJSON(worker, r)

	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		util.HttpRespondText(err.Error(), w)
		return
	}

	if worker.Token == "" || worker.Token != g_info.Token {
		w.WriteHeader(http.StatusBadRequest)
		util.HttpRespondText("missing token", w)
		return
	}

	if worker.Project == "" {
		w.WriteHeader(http.StatusBadRequest)
		util.HttpRespondText("missing project name", w)
		return
	}

	code := common.ResponseOK
	task := g_q.Pop(worker)

	if task != nil {
		g_log.Write(task.PopLog())
	} else {
		code = common.ResponseNoTask
	}

	util.HttpRespondMarshalJSON(&common.CommonResponse{code, task}, w)
}

func status(w http.ResponseWriter, r *http.Request) {
	check_init()
	s := fmt.Sprintf("total: %d\nissued: %d\n", g_q.cnt_total, g_q.cnt_issued)
	util.HttpRespondText(s, w)
}

func log(w http.ResponseWriter, r *http.Request) {
	check_init()
	util.HttpRespondText(g_log.Read(), w)
}

func start_webservice() {
	http.HandleFunc("/push", push)
	http.HandleFunc("/pop", pop)
	http.HandleFunc("/status", status)
	http.HandleFunc("/log", log)
	http.ListenAndServe(g_info.Address(), nil)
}

func Forever() {
	g_q = MakeQ()
	g_log = util.MakeLog(200)
	info, err := common.NewServerInfo(kServerConfigFile)
	if err != nil {
		util.Fatal("can't open config file %s\n", kServerConfigFile)
	}
	g_info = info
	start_webservice()
}
