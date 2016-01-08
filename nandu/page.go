package nandu

import ()

type TaskPageData struct {
	ID      uint   `gorm:"primary_key"`
	Cnt     int64  `sql:"-" json:"cnt"`
	Offset  int64  `sql:"-" json:"offset"`
	Min     int64  `sql:"-" json:"stop"`
	Type    string `sql:"not null,unique" json:"type"`
	Name    string `sql:"not null" json:"name"`
	Current int64  `sql:"not null" json:"min"`
	nomore  bool   `sql:"-"`
}

func (p *TaskPageData) HasMore() bool {
	return !p.nomore && p.Current > 1
}

func (p *TaskPageData) hasCurrent() bool {
	return p.Current != 0
}

func (p *TaskPageData) updateCurrent(current int64) {
	if !p.hasCurrent() || current < p.Current {
		p.Current = current
	}
	p.Cnt += 1
}

func (p *TaskPageData) Update(begin int64, end int64) (int64, int64) {

	if begin <= p.Min {
		p.nomore = true
		return 0, 0
	}

	if end <= p.Min {
		p.nomore = true
		end = p.Min + 1
	}

	if !p.hasCurrent() {
		p.updateCurrent(end)
		return 0, begin - end + 1
	} else {
		if end >= p.Current {
			return 0, 0
		} else if begin >= p.Current {
			prev := p.Current
			offset := begin - prev + 1
			p.updateCurrent(end)
			return offset, prev - end
		} else {
			p.updateCurrent(end)
			return 0, begin - end + 1
		}
	}
}
