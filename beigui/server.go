package beigui

import (
	"fmt"
	"github.com/Jiajun-Fan/nandu/util"
	"net/http"
)

var g_q *Q = nil
var g_log *util.Log = nil

func push(w http.ResponseWriter, r *http.Request) {
	if g_q == nil || g_log == nil {
		panic("not initialized")
	}
	g_log.Write("push\n")
	fmt.Fprintf(w, "push")
}

func pop(w http.ResponseWriter, r *http.Request) {
	if g_q == nil || g_log == nil {
		panic("not initialized")
	}
	g_log.Write("pop\n")
	fmt.Fprintf(w, "pop")
}

func status(w http.ResponseWriter, r *http.Request) {
	if g_q == nil || g_log == nil {
		panic("not initialized")
	}
	fmt.Fprintf(w, "total: %d\nissued: %d\n", g_q.Total(), g_q.Issued())
	fmt.Fprintf(w, "%s", g_log.Read())
}

func Forever() {
	g_q = MakeQ()
	g_log = util.MakeLog(200)
	http.HandleFunc("/push", push)
	http.HandleFunc("/pop", pop)
	http.HandleFunc("/status", status)
	http.ListenAndServe(":8080", nil)
}
