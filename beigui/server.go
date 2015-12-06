package beigui

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/ungerik/go-dry"
	"net/http"
)

var g_q *Q = nil
var g_log *util.Log = nil

func check_init() {
	if g_q == nil || g_log == nil {
		panic("not initialized")
	}
}

func push(w http.ResponseWriter, r *http.Request) {
	check_init()

	task := new(common.Task)
	err := util.HttpRequestUnmarshalJSON(task, r)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		dry.HTTPRespondText(err.Error(), w, r)
		return
	}

	if task.Project == "" {
		w.WriteHeader(http.StatusBadRequest)
		dry.HTTPRespondText("missing project name", w, r)
		return
	}

	if task.TaskSet == "" {
		w.WriteHeader(http.StatusBadRequest)
		dry.HTTPRespondText("missing taskset name", w, r)
		return
	}

	if task.Url == "" {
		w.WriteHeader(http.StatusBadRequest)
		dry.HTTPRespondText("missing taskset name", w, r)
		return
	}

	g_q.Push(task)

	g_log.Write(task.PushLog())

	dry.HTTPRespondMarshalJSON(&common.PushResponse{"OK", "task pushed"}, w, r)
}

func pop(w http.ResponseWriter, r *http.Request) {
	check_init()

	worker := new(common.Worker)
	err := util.HttpRequestUnmarshalJSON(worker, r)

	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		dry.HTTPRespondText(err.Error(), w, r)
		return
	}

	if worker.Project == "" {
		w.WriteHeader(http.StatusBadRequest)
		dry.HTTPRespondText("missing project name", w, r)
		return
	}

	task := g_q.Pop(worker)
	if task == nil {
		return
	}

	dry.HTTPRespondMarshalJSON(task, w, r)

	g_log.Write(task.PopLog())
}

func status(w http.ResponseWriter, r *http.Request) {
	check_init()
	var l common.LogResponse
	l.Issued = g_q.Issued()
	l.Total = g_q.Total()
	l.Logs = g_log.Read()
	dry.HTTPRespondText(l.Logs, w, r)
}

func start_webservice() {
	http.HandleFunc("/push", push)
	http.HandleFunc("/pop", pop)
	http.HandleFunc("/status", status)
	http.ListenAndServe(":8080", nil)
}

func Forever() {
	g_q = MakeQ()
	g_log = util.MakeLog(200)
	start_webservice()
}
