package main

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/nandu"
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/jinzhu/gorm"
	"time"
)

const (
	kDatabaseName = "nandu"
)

func main() {
	util.SetDebug(util.DebugInfo)

	worker := nandu.NewWorker()

	worker.Run()
	//worker.Model(kDatabaseName, &AwUser{})
}
