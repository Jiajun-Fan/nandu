package main

import (
	"github.com/Jiajun-Fan/nandu"
)

func main() {
	nandu.SetDebug(nandu.DebugDebug)
	q := nandu.MakeQ(2)
	q.AddWorker()
	q.Run()
}
