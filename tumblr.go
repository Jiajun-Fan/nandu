package nandu

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
)

type TumblrPage struct {
	base     string
	url      string
	api      *API
	oauth    *http.Client
	database *Database
	page     int
	retry    int
}

func NewTumblrPages(json *JsonTasks) []Task {
	client := NewOauth(&json.Oauth)
	database := NewDatabase(&json.Database)
	ret := make([]Task, 0, 0)
	if client == nil {
		return nil
	} else {
		for i := range json.Apis {
			api := json.Apis[i]
			for j := range api.Roots {
				blogger := api.Roots[j]
				task := new(TumblrPage)
				task.base = fmt.Sprintf("%s/%s.tumblr.com/%s", json.BaseUrl, blogger, api.Name)
				task.url = fmt.Sprintf("%s/%s.tumblr.com/%s", json.BaseUrl, blogger, api.Name)
				task.api = &api
				task.oauth = client
				task.database = database
				task.retry = 3
				ret = append(ret, task)
			}
		}
		return ret
	}
}

func (task *TumblrPage) Run(q *Q) (ret []Task) {

	retry := task
	ret = make([]Task, 0, 0)

	defer func() {
		if retry != nil && retry.retry > 0 {
            retry.retry -= 1
            Debug().V(DebugWarning).Printf("retry %s\n", task.url)
			ret = append(ret, retry)
		}
	}()

	defer Pass()

	r, err := task.oauth.Get(task.url)
	AssertNil(err)

	defer r.Body.Close()
	bytes, err := ioutil.ReadAll(r.Body)
	AssertNil(err)

	resp := new(TumblrResponse)
	err = json.Unmarshal(bytes, resp)

	Debug().V(DebugInfo).Printf("%s %d\n", task.url, resp.Data.Blog.Posts)

	newposts := 0
	nophoto := true
	for i := range resp.Data.Posts {
		rp := resp.Data.Posts[i]
        if rp.Type != "photo" {
            continue
        }
        nophoto = false
		newPost := new(TumblrSqlPost)
		task.database.db.Where("pid = ?", rp.Pid).First(newPost)
		if task.database.db.NewRecord(newPost) {
			newposts++
			post := NewTumblrSqlPost(&rp)
			task.database.db.Create(post)
			if task.database.db.NewRecord(post) {
				Debug().V(DebugError).Printf("can't create posts id %d url %s\n", post.Pid, task.url)
			}
		}
	}
	task.page += 20
	if resp.Data.Blog.Posts >= task.page && (newposts > 0 || nophoto == true) {
		task.url = fmt.Sprintf("%s?offset=%d", task.base, task.page)
		task.retry = 3
		ret = append(ret, task)
	}
	retry = nil

	return ret
}

type TumblrResponseSize struct {
	Url    string `json:"url"`
	Width  int    `json:"width"`
	Height int    `json:"height"`
}

type TumblrResponsePhoto struct {
	AltSizes []TumblrResponseSize `json:"alt_sizes"`
	OrigSize TumblrResponseSize   `json:"original_size"`
}

type TumblrResponsePost struct {
	Pid    uint64                `json:"id"`
	Type   string                `json:"type"`
	Title  string                `json:"source_title"`
	Photos []TumblrResponsePhoto `json:"photos"`
}

type TumblrResponseMeta struct {
	Status int    `json:"status"`
	Msg    string `json:"msg"`
}

type TumblrResponseBlog struct {
	Posts   int   `json:"posts"`
	Updates int64 `json:"update"`
	Likes   int   `json:"likes"`
}

type TumblrResponseData struct {
	Blog  TumblrResponseBlog   `json:"blog"`
	Posts []TumblrResponsePost `json:posts"`
}

type TumblrResponse struct {
	Meta TumblrResponseMeta `json:"meta"`
	Data TumblrResponseData `json:"response"`
}

type TumblrSqlPhoto struct {
	Id     uint
	Url    string `json:"url"`
	Width  int    `json:"width"`
	Height int    `json:"height"`
    Done bool
}

type TumblrSqlPost struct {
	Id     uint
	Pid    uint64
	Title  string
	Photos []TumblrSqlPhoto
}

func NewTumblrSqlPost(r *TumblrResponsePost) *TumblrSqlPost {
	post := new(TumblrSqlPost)
	post.Photos = make([]TumblrSqlPhoto, 0, 0)
	for i := range r.Photos {
		rp := r.Photos[i]
		var photo TumblrSqlPhoto
		photo.Url = rp.OrigSize.Url
		photo.Width = rp.OrigSize.Width
		photo.Height = rp.OrigSize.Height
        photo.Done = false
		post.Photos = append(post.Photos, photo)
	}
	post.Pid = r.Pid
	post.Title = r.Title

	return post
}
