package main

import (
	"fmt"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/nandu"
	"github.com/Jiajun-Fan/nandu/util"
)

const (
	kProjectName = "cool project"
	kTaskSetName = "cool taskset"
)

func PrintResponse(task *common.Task, bytes []byte) error {
	fmt.Printf("%s", string(bytes[:]))
	return nil
}

func main() {
	util.SetDebug(util.DebugInfo)
	worker := nandu.NewWorker(kProjectName)
	worker.RegisterParser(kTaskSetName, PrintResponse)

	task := new(common.Task)
	task.Project = "cool project"
	task.TaskSet = "cool taskset"
	task.Url = "http://www.baidu.com"
	worker.Push(task)

	worker.Run()
}
