package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"io/ioutil"
	"net/http"
)

type ApiClient struct {
	oauth    *http.Client
	database *Database
	name     string
}

func NewApiClient(name string, appKey string, appSecret string, token string, secret string) *ApiClient {
	client := new(ApiClient)
	oauth, err := NewOauth(appKey, appSecret, token, secret)
	if err != nil {
		util.Debug().Fatal("%v\n", err)
	}
	client.name = name
	client.oauth = oauth
	return client
}

func (c *ApiClient) Name(task *common.Task) string {
	return c.name
}

func (c *ApiClient) Run(task *common.Task) []byte {
	r, err := c.oauth.Get(task.Url)
	if err != nil {
		util.Debug().Error("can't fetch url %s\n", task.Url)
		return nil
	}

	defer r.Body.Close()

	bytes, err := ioutil.ReadAll(r.Body)

	if err != nil {
		util.Debug().Error("can't get bytes from url %s\n")
		return nil
	}

	if r.StatusCode/100 != 2 {
		util.Debug().Error("get status code %d\n", r.StatusCode)
		return nil
	}

	return bytes
}
