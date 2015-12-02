package beigui

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/emicklei/go-restful"
	"net/http"
)

var g_q *Q = nil
var g_log *util.Log = nil

func check_init() {
	if g_q == nil || g_log == nil {
		panic("not initialized")
	}
}

func badrequest(msg string, r *restful.Response) {
	r.WriteErrorString(http.StatusBadRequest, msg)
}

func push(q *restful.Request, r *restful.Response) {
	check_init()

	task := new(common.Task)
	err := q.ReadEntity(task)
	if err != nil {
		badrequest(err.Error(), r)
		return
	}

	if task.Project == "" {
		badrequest("missing project name", r)
		return
	}

	if task.TaskSet == "" {
		badrequest("missing taskset name", r)
		return
	}
	if task.Url == "" {
		badrequest("missing taskset name", r)
		return
	}

	g_q.Push(task)

	g_log.Write(task.PushLog())

	r.WriteEntity(&common.PushResponse{"OK", "task pushed"})
}

func pop(q *restful.Request, r *restful.Response) {
	check_init()

	worker := new(common.Worker)
	err := q.ReadEntity(worker)

	if err != nil {
		badrequest(err.Error(), r)
		return
	}

	if worker.Project == "" {
		badrequest("missing project name", r)
		return
	}

	task := g_q.Pop(worker)
	if task == nil {
	}

	r.WriteEntity(task)

	g_log.Write(task.PopLog())
}

func status(q *restful.Request, r *restful.Response) {
	check_init()
	var l common.LogResponse
	l.Issued = g_q.Issued()
	l.Total = g_q.Total()
	l.Logs = g_log.Read()
	r.WriteEntity(&l)
}

func init_webservice() {
	ws := new(restful.WebService)
	ws.Consumes(restful.MIME_JSON).Produces(restful.MIME_JSON)
	ws.Route(ws.PUT("/push").To(push))
	ws.Route(ws.POST("/pop").To(pop))
	ws.Route(ws.GET("/status").To(status))
	restful.Add(ws)
}

func Forever() {
	g_q = MakeQ()
	g_log = util.MakeLog(200)
	init_webservice()
	http.ListenAndServe(":8080", nil)
}
