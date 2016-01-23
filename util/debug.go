package util

import (
	"fmt"
	"os"
)

type Debugger int

var gDebug Debugger = DebugNull

func SetDebug(d Debugger) {
	if d < DebugNull || d > DebugDebug {
		panic("illegal debug value")
	}
	gDebug = d
}

const (
	DebugNull    Debugger = 0
	DebugFatal   Debugger = 1
	DebugError   Debugger = 2
	DebugWarning Debugger = 3
	DebugInfo    Debugger = 4
	DebugDebug   Debugger = 5
)

func (d Debugger) V(v Debugger) Debugger {
	if d >= v {
		if v == DebugDebug {
			fmt.Printf("[  Debug  ]: ")
		} else if v == DebugInfo {
			fmt.Printf("[  Info   ]: ")
		} else if v == DebugWarning {
			fmt.Printf("[ Warning ]: ")
		} else if v == DebugError {
			fmt.Printf("[  Error  ]: ")
		} else if v == DebugFatal {
			fmt.Printf("[  Fatal ]: ")
		}
		return d
	} else {
		return DebugNull
	}
}

func (d Debugger) Printf(fmts string, args ...interface{}) {
	if d > 0 {
		fmt.Printf(fmts, args...)
	}
}

func Fatal(fmts string, args ...interface{}) {
	if gDebug >= 0 {
		gDebug.V(DebugFatal).Printf(fmts, args...)
	}
	os.Exit(-1)
}

func Error(fmts string, args ...interface{}) {
	if gDebug >= 0 {
		gDebug.V(DebugError).Printf(fmts, args...)
	}
}

func Warning(fmts string, args ...interface{}) {
	if gDebug >= 0 {
		gDebug.V(DebugWarning).Printf(fmts, args...)
	}
}

func Info(fmts string, args ...interface{}) {
	if gDebug >= 0 {
		gDebug.Printf(fmts, args...)
	}
}

func Debug(fmts string, args ...interface{}) {
	if gDebug >= 0 {
		gDebug.V(DebugDebug).Printf(fmts, args...)
	}
}
