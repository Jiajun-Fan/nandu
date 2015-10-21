package main

import (
	"nandu"
)

func main() {
	nandu.SetDebug(nandu.DebugDebug)
	q := nandu.MakeQ(2)
	q.AddWorker()
	q.Run()
}
