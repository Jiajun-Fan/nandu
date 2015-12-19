package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
)

type Client interface {
	Run(task *common.Task)
	Name() string
}
