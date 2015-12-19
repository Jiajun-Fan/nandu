package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
)

type ParserHandler func(task *common.Task, bytes []byte) error
