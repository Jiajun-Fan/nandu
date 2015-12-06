package nandu

type JsonOauth struct {
	Type      string `json:"type"`
	Name      string `json:"name"`
	AppKey    string `json:"appkey"`
	AppSecret string `json:"appsecret"`
	Token     string `json:"token"`
	Secret    string `json:"secret"`
}
