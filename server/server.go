package server

import (
	"fmt"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"net/http"
)

var g_info *ServerInfo = nil
var g_q *Q = nil

func check_init() {
	if g_q == nil || g_info == nil {
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

	g_q.push(task)

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
	task := g_q.pop(worker)

	if task == nil {
		code = common.ResponseNoTask
	}

	util.HttpRespondMarshalJSON(&common.CommonResponse{code, task}, w)
}

func status(w http.ResponseWriter, r *http.Request) {
	check_init()
	s := fmt.Sprintf("total: %d\nissued: %d\n", g_q.cnt_total, g_q.cnt_issued)
	util.HttpRespondText(s, w)
}

func start_webservice() {
	http.HandleFunc("/push", push)
	http.HandleFunc("/pop", pop)
	http.HandleFunc("/status", status)
	http.ListenAndServe(g_info.address(), nil)
}

func Forever() {
	g_q = newQ()
	info, err := newServerInfo()
	if err != nil {
		genServerInfoTemplate()
		util.Fatal("can't open config file '%s', please fill the information in '%s.template', and rename it to '%s'\n", kServerConfigFile, kServerConfigFile, kServerConfigFile)
	}
	g_info = info
	start_webservice()
}
