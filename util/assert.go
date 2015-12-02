package util

import ()

type Error struct {
	fatal bool
	msg   string
}

func AssertErr(e error) {
	if e != nil {
		panic(Error{false, e.Error()})
	}
}

func Assert(b bool, t string) {
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
