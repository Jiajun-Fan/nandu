package main

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/nandu"
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/jinzhu/gorm"
	"time"
)

const (
	kDatabaseName           = "aw"
	kAwGroupInvalid AwGroup = 0
	kAwGroupFree    AwGroup = 1
	kAwGroupBasic   AwGroup = 2
	kAwGroupAdvan   AwGroup = 3
)

type AwGroup uint

type AwUser struct {
	common.User
	Group    AwGroup
	Expire   time.Time
	Token    string
	Secret   string
	ErrorCnt uint
}

func (user *AwUser) Validate(t time.Time, db *gorm.DB) {
	if db.NewRecord(user) {
		user.Group = kAwGroupInvalid
		return
	}

	if user.Token == "" || user.Secret == "" {
		user.Group = kAwGroupInvalid
		db.Save(user)
		return
	}

	if user.Group > kAwGroupFree {
		if user.Expire.Before(t) {
			user.Group = kAwGroupFree
			db.Save(user)
		}
	}
}

func AddUserTasks(db *gorm.DB) {
	t := time.Now()
	users := make([]AwUser, 0)
	db.Find(&users)

	for i := range users {
		user := users[i]
		user.Validate(t, db)
		if user.Group >= kAwGroupFree {
			util.Info("pushing user %s (%d) into queue\n", user.Name, user.Group)
		}
	}
}

func main() {
	util.SetDebug(util.DebugInfo)

	worker := nandu.NewWorker()

	worker.Model(kDatabaseName, &AwUser{})

	t := time.Now().AddDate(0, 1, 0)

	/*user := AwUser{
		User: common.User{
			Email: "celeheaven@gmail.com",
			Name:  "Jiajun-Fan",
		},
		Group:  kAwGroupAdvan,
		Expire: t,
		Token:  "J3rWxdFvpbur26UljYASM6ou1R4XYhpkmGQ0RZqYw3HHhY0v4P",
		Secret: "afBwgNzPI6LWRtacBmWQ7kvX2vEtFPQQGROvGlZV4TtSshG0sH",
	}

	worker.GetDB(kDatabaseName).Create(&user)*/

	AddUserTasks(worker.GetDB(kDatabaseName))
}
