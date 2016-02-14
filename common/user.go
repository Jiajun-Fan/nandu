package common

type User struct {
	ID    int64  `gorm:"primary_key"`
	Email string `sql:"not null;unique"`
	Name  string `sql:"not null"`
}
