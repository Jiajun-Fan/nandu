package nandu

import (
	"bufio"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"os"
)

type DownloadData struct {
	FileName string `json:"filename"`
}

func DownloadParser(worker *Worker, task *common.Task, bytes []byte) {

	task.FillData(DownloadData{})
	filename := task.Data.(DownloadData).FileName

	if filename == "" {
		util.Debug().Error("no file name specified\n")
		return
	}

	if f, err := os.Create(filename); err != nil {
		util.Debug().Error("%s\n", err.Error())
	} else {
		defer f.Close()

		w := bufio.NewWriter(f)
		w.Write(bytes)
		w.Flush()
		util.Debug().Info("downloading %s\n", task.Url)
	}
}
