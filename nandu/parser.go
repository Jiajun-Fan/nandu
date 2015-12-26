package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
)

type ParserHandler func(worker *Worker, task *common.Task, bytes []byte)
