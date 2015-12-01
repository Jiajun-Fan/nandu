package util

import ()

type Error struct {
	fatal bool
	msg   string
}

func AssertNil(e error) {
	if e != nil {
		panic(Error{false, e.Error()})
	}
}

func AssertTrue(t string, b bool) {
	if !b {
		panic(Error{false, t})
	}
}

func Pass() {
	e := recover()
	if e != nil {
		if err, ok := e.(Error); ok {
			if !err.fatal {
				debug.Error(err.msg)
			}
		} else {
			panic(e)
		}
	}
}
