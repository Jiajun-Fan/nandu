package util

import (
	"bytes"
	"sync"
)

type Log struct {
	size  uint64
	index uint64
	buff  []string
	full  bool
	lock  sync.Mutex
}

func (l *Log) Write(s string) {
	l.lock.Lock()
	l.buff[l.index] = s
	l.index += 1
	if l.index == l.size {
		l.full = true
		l.index = 0
	}
	l.lock.Unlock()
}

func (l *Log) Read() string {
	var buff bytes.Buffer
	if l.full {
		for i := l.index; i < l.size; i++ {
			buff.WriteString(l.buff[i])
		}
	}
	for i := uint64(0); i < l.index; i++ {
		buff.WriteString(l.buff[i])
	}
	return buff.String()
}

func MakeLog(s uint64) *Log {
	log := new(Log)
	log.size = s
	log.buff = make([]string, s, s)
	log.full = false
	log.index = 0
	return log
}
