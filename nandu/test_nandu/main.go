package main

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/nandu"
	"github.com/Jiajun-Fan/nandu/util"
)

func main() {
	util.SetDebug(util.DebugInfo)
	worker := nandu.NewWorker("very cool worker")
	task := new(common.Task)
	task.Project = "cool project"
	task.TaskSet = "cool taskset"
	task.Url = "http://google.com"
	worker.Push(task)
	worker.Pop()
}
