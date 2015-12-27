package nandu

import (
	"github.com/jinzhu/gorm"
	_ "github.com/mattn/go-sqlite3"
)

type Database *gorm.DB

func NewDatabase(dbType string, connect string) (Database, error) {
	db, err := gorm.Open(dbType, connect)
	if err != nil {
		return nil, err
	}
	return &db, nil
}
