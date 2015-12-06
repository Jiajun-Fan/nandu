package main

import (
	"github.com/Jiajun-Fan/nandu/beigui"
	"github.com/Jiajun-Fan/nandu/util"
)

func main() {
	util.SetDebug(util.DebugError)
	beigui.Forever()
}
