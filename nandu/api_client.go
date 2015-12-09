package nandu

import (
	"github.com/Jiajun-Fan/nandu/util"
	"net/http"
)

type ApiClient struct {
	oauth    *http.Client
	database *Database
}

func NewApiClient(appKey string, appSecret string, token string, secret string) *ApiClient {
	client := new(ApiClient)
	oauth, err := NewOauth(appKey, appSecret, token, secret)
	if err != nil {
		util.Debug().Fatal("%v\n", err)
	}
	client.oauth = oauth
	return client
}

func (ApiClient *c) Run(task *common.Task) {
}
