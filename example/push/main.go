package main

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/nandu"
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/jinzhu/gorm"
)

const (
	kTaskPushInfoFile    = "tasks.json"
	kDatabaseName        = "nandu"
	kDownloadTaskSetName = "tumblr_download"
)

type DownloadData struct {
	Pid uint `json:"pid"`
}

type TaskTumblrData struct {
	nandu.TaskPageData
	Bid   int64 `json:"bid"`
	Sleep int64 `json:"sleep"`
}

type TaskFileDataRange struct {
	Start uint `json:"start"`
	Stop  uint `json:"stop"`
}

type TumblrPhoto struct {
	ID           uint `json:"-" gorm:"primary_key"`
	TumblrPostID uint
	FileDataID   uint
	Url          string
	Width        int
	Height       int
}

type TaskPushInfo struct {
	Blogs         []common.Task     `json:"blogs"`
	FileDataRange TaskFileDataRange `json:"file_data_range"`
}

func NewTaskPushInfo(file string) (*TaskPushInfo, error) {
	info := new(TaskPushInfo)
	err := util.ReadJsonFile(file, info)
	if err != nil {
		return nil, err
	}
	return info, err
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

func getPhotos(start uint, stop uint, db *gorm.DB) []TumblrPhoto {
	if start >= stop {
		return nil
	}
	photos := make([]TumblrPhoto, 0, 0)
	db.Where("id >= ? and id < ?", start, stop).Find(&photos)
	return photos
}

func main() {
	util.SetDebug(util.DebugInfo)

	worker := nandu.NewWorker()

	info, err := NewTaskPushInfo(kTaskPushInfoFile)
	if err != nil {
		util.Fatal("%s\n", err.Error())
	}

	for i := range info.Blogs {
		task := &info.Blogs[i]
		d := TaskTumblrData{}
		task.GetData(&d)

		d.Min = getStop(d.Name, worker.GetDB(kDatabaseName))
		task.SetData(d)
		util.Info("%s", task.PushLog())
		worker.Push(task)
	}

	photos := getPhotos(info.FileDataRange.Start, info.FileDataRange.Stop, worker.GetDB(kDatabaseName))
	if photos != nil {
		for i := range photos {
			task := common.Task{}
			if photos[i].FileDataID != 0 {
				continue
			}
			task.Url = photos[i].Url
			task.TaskSet = kDownloadTaskSetName
			task.SetData(DownloadData{photos[i].ID})
			util.Info("push %s\n", task.Url)
			worker.Push(&task)
		}
	}
}
