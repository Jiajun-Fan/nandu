package nandu

import ()

type PageInterval struct {
	ID     uint   `gorm:"primary_key"`
	Cnt    int64  `sql:"-"`
	Size   int64  `sql:"-"`
	Offset int64  `sql:"-"`
	Type   string `sql:"not null,unique"`
	Name   string `sql:"not null"`
	Min    int64  `sql:"not null"`
	Max    int64  `sql:"not null"`
}

func NewPageInterval(t string, name string, max int64, min int64, offset int64, size int64, cnt int64) *PageInterval {
	p := new(PageInterval)
	p.Type = t
	p.Name = name
	p.Max = max
	p.Min = min
	p.Size = size
	p.Offset = offset
	p.Cnt = cnt
	return p
}

func (p *PageInterval) Update(total int64, offset int64, db Database) {
	current := total - offset
	if current > p.Max {
		p.Max = current
	}
	if current < p.Min || p.Min < 0 {
		p.Min = current
	}
	p.Cnt += 1
	// save to database
}
