package main

import (
	"github.com/Jiajun-Fan/nandu/server"
	"github.com/Jiajun-Fan/nandu/util"
)

func main() {
	util.SetDebug(util.DebugError)
	server.Forever()
}
