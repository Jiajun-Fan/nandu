package nandu

import (
	"github.com/jinzhu/gorm"
	_ "github.com/mattn/go-sqlite3"
)

type Database struct {
	db *gorm.DB
}

func NewDatabase(json *JsonDatabase) *Database {
	defer Pass()
	database := new(Database)
	db, err := gorm.Open(json.Type, json.Data)
	AssertNil(err)
	database.db = &db
	if json.Init {
		db.CreateTable(&TumblrSqlPhoto{})
		db.CreateTable(&TumblrSqlPost{})
		db.CreateTable(&TumblrSqlBlog{})
	}
	return database
}
