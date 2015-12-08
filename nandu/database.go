package nandu

import (
	"github.com/jinzhu/gorm"
	_ "github.com/mattn/go-sqlite3"
)

type Database *gorm.DB

func NewDatabase(json *JsonDatabase) *Database {
	db, err := gorm.Open(json.Type, json.Data)
	if err != nil {
		Debug().Fatal("Can't connect to database %v", err)
	}
	return db
}
