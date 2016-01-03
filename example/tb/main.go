package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/nandu"
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/jinzhu/gorm"
	"os"
	"strings"
	"time"
)

const (
	kTumblrTaskSetName   = "tumblr_api"
	kDownloadTaskSetName = "tumblr_download"
)

type TaskTumblrData struct {
	nandu.TaskPageData
	Bid      int64 `json:"bid"`
	Download bool  `json:download"`
	Sleep    int64 `json:"sleep"`
}

type TumblrBlog struct {
	ID          uint         `json:"-" gorm:"primary_key"`
	Name        string       `json:"name"`
	Posts       int64        `json:"posts" sql:"-"`
	TumblrPosts []TumblrPost `json:"-"`
}

type TumblrPost struct {
	ID           uint          `json:"-" gorm:"primary_key"`
	TumblrBlogID uint          `json:"-"`
	Offset       uint          `json:"-"`
	Type         string        `json:"type" sql:"-"`
	Title        string        `json:"source_title"`
	TumblrPhotos []TumblrPhoto `json:"photos"`
}

type TumblrPhoto struct {
	ID           uint   `json:"-" gorm:"primary_key"`
	TumblrPostID uint   `json:"-"`
	Url          string `json:"-"`
	Width        int    `json:"-"`
	Height       int    `json:"-"`
	Orig         struct {
		Url    string `json:"url"`
		Width  int    `json:"width"`
		Height int    `json:"height"`
	} `json:"original_size" sql:"-"`
}

type TumblrResponse struct {
	Data struct {
		Blog  TumblrBlog   `json:"blog"`
		Posts []TumblrPost `json:"posts"`
	} `json:"response"`
}

func (p *TumblrPhoto) Fill() {
	p.Url = p.Orig.Url
	p.Width = p.Orig.Width
	p.Height = p.Orig.Height
}

func getTumblrBlog(name string, db *gorm.DB) *TumblrBlog {
	blog := new(TumblrBlog)
	db.FirstOrCreate(blog, TumblrBlog{Name: name})
	return blog
}

func genUrlFromInterval(d *TaskTumblrData) string {
	return fmt.Sprintf("http://api.tumblr.com/v2/blog/%s.tumblr.com/posts?offset=%d", d.Name, d.Offset)
}

func TumblrParser(worker *nandu.Worker, task *common.Task, bytes []byte) {
	resp := new(TumblrResponse)
	err := json.Unmarshal(bytes, resp)
	if err != nil {
		util.Debug().Error("failed to parse response\n")
		return
	}

	d := TaskTumblrData{}
	task.GetData(&d)
	d.Type = task.TaskSet

	if d.Sleep != 0 {
		time.Sleep(time.Duration(d.Sleep) * time.Millisecond)
	}

	if d.Bid == 0 {
		blog := getTumblrBlog(d.Name, worker.GetDB())
		d.Bid = int64(blog.ID)
	}

	util.Debug().Info("fetching %s\n", task.Url)

	begin := int64(resp.Data.Blog.Posts) - d.Offset
	end := begin - int64(len(resp.Data.Posts)) + 1

	ibegin, iend := d.Update(begin, end)

	for i := ibegin; i < iend; i++ {
		post := resp.Data.Posts[i]
		post.TumblrBlogID = uint(d.Bid)
		post.Offset = uint(begin - i)
		for j := range post.TumblrPhotos {
			post.TumblrPhotos[j].Fill()
			url := post.TumblrPhotos[j].Orig.Url
			if fn, err := getFileName(url); err == nil {
				util.Debug().Info("yield %s %s (%d | %d)\n", url, fn, resp.Data.Blog.Posts, begin-i)
				if d.Download {
					dtask := new(common.Task)
					dtask.Project = task.Project
					dtask.TaskSet = kDownloadTaskSetName
					dtask.Url = url
					dtask.Data = nandu.DownloadData{FileName: fn}
					worker.Push(dtask)
				}
			}
		}
		worker.GetDB().Create(&post)
	}

	if d.HasMore() {
		new_task := new(common.Task)
		new_task.Project = task.Project
		new_task.TaskSet = task.TaskSet
		d.Offset = int64(resp.Data.Blog.Posts) - d.Min + 1
		new_task.Data = d
		new_task.Url = genUrlFromInterval(&d)
		worker.Push(new_task)
	}
}

func getFileName(url string) (string, error) {
	index := strings.LastIndex(url, "/")
	if index == -1 {
		return "", errors.New(fmt.Sprintf("can't find filename from url %s", url))
	}
	pwd, _ := os.Getwd()
	filename := fmt.Sprintf("%s/tumblr/%s", pwd, url[index+1:])
	return filename, nil
}

func main() {
	util.SetDebug(util.DebugInfo)

	worker := nandu.NewWorker()

	worker.RegisterParser(kTumblrTaskSetName, TumblrParser)
	worker.RegisterParser(kDownloadTaskSetName, nandu.DownloadParser)
	worker.RegisterModel(&TumblrBlog{})
	worker.RegisterModel(&TumblrPhoto{})
	worker.RegisterModel(&TumblrPost{})
	worker.RegisterModel(&nandu.FileData{})

	worker.Run()
}
