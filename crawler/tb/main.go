package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/nandu"
	"github.com/Jiajun-Fan/nandu/util"
	"strings"
)

const (
	kTumblrTaskSetName   = "tumblr_api"
	kDownloadTaskSetName = "tumblr_download"
)

type TumblrResponsePhoto struct {
	Orig struct {
		Url    string `json:"url"`
		Width  int    `json:"width"`
		Height int    `json:"height"`
	} `json:"original_size"`
}

type TumblrResponse struct {
	Data struct {
		Blog struct {
			Name    string `json:"name"`
			Posts   int    `json:"posts"`
			Updates int64  `json:"update"`
			Likes   int    `json:"likes"`
		} `json:"blog"`
		Posts []struct {
			Pid    uint64                `json:"id"`
			Type   string                `json:"type"`
			Title  string                `json:"source_title"`
			Photos []TumblrResponsePhoto `json:"photos"`
		} `json:"posts"`
	} `json:"response"`
}

func genUrlFromInterval(pi *nandu.PageInterval) string {
	return fmt.Sprintf("http://api.tumblr.com/v2/blog/%s.tumblr.com/posts?offset=%d", pi.Name, pi.Offset)
}

func TumblrParser(worker *nandu.Worker, task *common.Task, bytes []byte) {
	resp := new(TumblrResponse)
	err := json.Unmarshal(bytes, resp)
	if err != nil {
		util.Debug().Error("failed to parse response\n")
		return
	}

	task.FillData(nandu.PageInterval{})

	pi := task.Data.(nandu.PageInterval)

	util.Debug().Info("fetching %s\n", task.Url)

	offset := pi.Offset

	for i := range resp.Data.Posts {
		pi.Update(int64(resp.Data.Blog.Posts), offset+int64(i), nil)
		//pi.Update(int64(resp.Data.Blog.Posts), offset+int64(i), worker.GetDB())
		for j := range resp.Data.Posts[i].Photos {
			url := resp.Data.Posts[i].Photos[j].Orig.Url
			dtask := new(common.Task)
			dtask.Project = task.Project
			dtask.TaskSet = kDownloadTaskSetName
			dtask.Url = url
			if fn, err := getFileName(url); err == nil {
				dtask.Data = nandu.DownloadData{FileName: fn}

				util.Debug().Info("yield %s %s (%d | %d)\n", url, fn, resp.Data.Blog.Posts, pi.Min)
				worker.Push(dtask)
			}
		}
	}
	if pi.Min > 1 {
		new_task := new(common.Task)
		new_task.Project = task.Project
		new_task.TaskSet = task.TaskSet
		pi.Offset = int64(resp.Data.Blog.Posts) - pi.Min + 1
		new_task.Data = pi
		new_task.Url = genUrlFromInterval(&pi)
		worker.Push(new_task)
	}
}

func getFileName(url string) (string, error) {
	index := strings.LastIndex(url, "/")
	if index == -1 {
		return "", errors.New(fmt.Sprintf("can't find filename from url %s", url))
	}
	return url[index+1:], nil
}

func main() {
	util.SetDebug(util.DebugInfo)

	worker := nandu.NewWorker()

	worker.RegisterParser(kTumblrTaskSetName, TumblrParser)
	worker.RegisterParser(kDownloadTaskSetName, nandu.DownloadParser)

	worker.Run()
}
