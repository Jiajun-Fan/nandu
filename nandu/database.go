package nandu

import (
	"github.com/Jiajun-Fan/nandu/util"
	_ "github.com/go-sql-driver/mysql"
	"github.com/jinzhu/gorm"
	_ "github.com/lib/pq"
	_ "github.com/mattn/go-sqlite3"
)

type NanduDB struct {
	DB   *gorm.DB
	Init bool
}

func NewDatabase(t string, c string) *gorm.DB {
	if t == "" || c == "" {
		util.Error("db type and connect string can't be empty")
		return nil
	}
	if database, err := gorm.Open(t, c); err != nil {
		util.Error("can't connect to database %s\n", err.Error())
		return nil
	} else {
		return &database
	}
}
