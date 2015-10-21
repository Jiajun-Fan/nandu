package nandu

type JsonTasks struct {
	Type     string       `json:"type"`
	Name     string       `json:"name"`
	BaseUrl  string       `json:"baseurl"`
	Oauth    JsonOauth    `json:"oauth"`
	Apis     []API        `json:"apis"`
	Database JsonDatabase `json:"database"`
}

type JsonOauth struct {
	Type      string `json:"type"`
	Name      string `json:"name"`
	AppKey    string `json:"appkey"`
	AppSecret string `json:"appsecret"`
	Token     string `json:"token"`
	Secret    string `json:"secret"`
}

type JsonDatabase struct {
	Type string `json:"type"`
	Data string `json:"sqlcon"`
	Init bool   `json:"init"`
}
