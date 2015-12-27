package nandu

import (
	"github.com/Jiajun-Fan/nandu/util"
	"github.com/mrjones/oauth"
	"net/http"
)

func NewOauth(appKey string, appSecret string, token string, secret string) *http.Client {
	if token == "" || secret == "" || appKey == "" || appSecret == "" {
		util.Debug().Error("token can't be empty\n")
		return nil
	}
	consumer := oauth.NewConsumer(
		appKey,
		appSecret,
		oauth.ServiceProvider{
			"", "", "", "",
		})

	t := new(oauth.AccessToken)
	t.Token = token
	t.Secret = secret

	client, err := consumer.MakeHttpClient(t)

	if err != nil {
		util.Debug().Error("%s\n", err.Error())
		return nil
	}
	return client
}
