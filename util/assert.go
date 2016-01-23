package util

import ()

type Exception struct {
	fatal bool
	msg   string
}

func AssertErr(e error) {
	if e != nil {
		panic(Exception{false, e.Error()})
	}
}

func Assert(b bool, t string) {
	if !b {
		panic(Exception{false, t})
	}
}

func Pass() {
	e := recover()
	if e != nil {
		if err, ok := e.(Exception); ok {
			if !err.fatal {
				Error(err.msg)
			}
		} else {
			panic(e)
		}
	}
}
