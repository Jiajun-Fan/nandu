package nandu

import (
	"fmt"
)

type Debugger int

var debug Debugger = DebugNull

func Debug() Debugger {
	return debug
}

func DebugV(v Debugger) Debugger {
	return debug.V(v)
}

func SetDebug(d Debugger) {
	if d < DebugNull || d > DebugDebug {
		panic("illegal debug value")
	}
	debug = d
}

const (
	DebugNull    Debugger = 0
	DebugError   Debugger = 1
	DebugWarning Debugger = 2
	DebugInfo    Debugger = 3
	DebugDebug   Debugger = 4
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

func (d Debugger) Error(info string) {
	if d >= 0 {
		d.V(DebugError).Printf("%s\n", info)
	}
}

func (d Debugger) Warning(info string) {
	if d >= 0 {
		d.V(DebugWarning).Printf("%s\n", info)
	}
}

func (d Debugger) Info(info string) {
	if d >= 0 {
		d.V(DebugInfo).Printf("%s\n", info)
	}
}

func (d Debugger) Debug(info string) {
	if d >= 0 {
		d.V(DebugDebug).Printf("%s\n", info)
	}
}
