package nandu

import (
	"github.com/jinzhu/gorm"
)

type PageInterval struct {
	ID     uint   `gorm:"primary_key"`
	Cnt    int64  `sql:"-" json:"cnt"`
	Offset int64  `sql:"-" json:"offset"`
	Type   string `sql:"not null,unique" json:"type"`
	Name   string `sql:"not null" json:"name"`
	Min    int64  `sql:"not null" json:"min"`
	Max    int64  `sql:"not null" json:"max"`
}

func (p *PageInterval) hasCurrent() bool {
	return p.Max != 0
}

func (p *PageInterval) updateCurrent(begin int64, end int64) {
	if begin == 0 || end == 0 {
		panic("update with 0")
	}
	if begin > p.Max {
		p.Max = begin
	}
	if p.Min == 0 || end < p.Min {
		p.Min = end
	}
	p.Cnt += 1
}

func (p *PageInterval) Update(total int64, offset int64, length int64, db *gorm.DB) (int64, int64) {

	if length == 0 {
		return 0, 0
	}

	begin := total - offset
	end := total - offset - length + 1

	if !p.hasCurrent() {
		p.updateCurrent(begin, end)
		return 0, length
	} else {
		if end > p.Max {
			p.Min = 0
			p.updateCurrent(begin, end)
			return 0, length
		} else if p.Min > begin {
			p.updateCurrent(begin, end)
			return 0, length
		} else {
			if end < p.Min {
				p.updateCurrent(begin, end)
				return begin - p.Min - 1, length
			}
			return 0, 0
		}
	}
}
