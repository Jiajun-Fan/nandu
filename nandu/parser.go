package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
)

type ParserHandler func(taskset *TaskSet, task *common.Task, bytes []byte)
