package main

type TumblrMeta struct {
	Status string `json:"status"`
	Msg    string `json:"msg"`
}

/* /user/info */

type TumblrUserInfoResponse struct {
	Following         uint64               `json:"following"`
	DefaultPostFormat string               `json:"default_post_format"`
	Name              string               `json:"name"`
	Likes             string               `json:"likes"`
	Blogs             []TumblrUserInfoBlog `json:"blogs"`
}

type TumblrUserInfoBlog struct {
	Name      string `json:"name"`
	Title     string `json:"title"`
	Url       string `json:"url"`
	Primary   bool   `json:"primary"`
	Followers uint   `json:"followers"`
	Tweet     string `json:"tweet"`
	Facebook  string `json:"facebook"`
	Type      string `json:"type"`
}
