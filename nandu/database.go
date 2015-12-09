package nandu

import (
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/jinzhu/gorm"
	_ "github.com/mattn/go-sqlite3"
)

type Database *gorm.DB

func NewDatabase(dbType string, connect string) Database {
	db, err := gorm.Open(dbType, connect)
	if err != nil {
		util.Debug().Fatal("Can't connect to database %v", err)
	}
	return &db
}
