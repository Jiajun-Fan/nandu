package main

import (
	"github.com/Jiajun-Fan/nandu/nandu"
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/jinzhu/gorm"
)

type TaskTumblrData struct {
	nandu.TaskPageData
	Bid      int64 `json:"bid"`
	Download bool  `json:"download"`
	Sleep    int64 `json:"sleep"`
}

type TumblrBlog struct {
	ID          uint         `json:"-" gorm:"primary_key"`
	Name        string       `json:"name"`
	Posts       int64        `json:"posts" sql:"-"`
	TumblrPosts []TumblrPost `json:"-"`
}

type TumblrPost struct {
	ID           uint   `json:"-" gorm:"primary_key"`
	TumblrBlogID uint   `json:"-"`
	Offset       uint   `json:"-"`
	Type         string `json:"type" sql:"-"`
	Title        string `json:"source_title"`
}

func getStop(name string, db *gorm.DB) int64 {
	blog := TumblrBlog{}
	db.Where("name = ?", name).First(&blog)
	if db.NewRecord(blog) {
		return 0
	}
	post := TumblrPost{}
	db.Where("tumblr_blog_id = ?", blog.ID).Order("offset desc").First(&post)
	if db.NewRecord(post) {
		return 0
	}
	return int64(post.Offset)
}

func main() {
	util.SetDebug(util.DebugInfo)

	worker := nandu.NewWorker()
	info := worker.GetInfo()
	for i := range info.InitTasks {
		task := &info.InitTasks[i]
		d := TaskTumblrData{}
		task.GetData(&d)

		d.Stop = getStop(d.Name, worker.GetDB())
		task.Data = d
	}
	worker.PushInitTasks()
}
