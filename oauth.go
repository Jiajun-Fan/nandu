package nandu

import (
	"github.com/mrjones/oauth"
	"net/http"
)

func NewOauth(json *JsonOauth) *http.Client {
	if json.Token == "" || json.Secret == "" || json.AppKey == "" || json.AppSecret == "" {
		Debug().Error("token can't be empty!\n")
		return nil
	}
	consumer := oauth.NewConsumer(
		json.AppKey,
		json.AppSecret,
		oauth.ServiceProvider{
			"", "", "", "",
		})

	token := new(oauth.AccessToken)
	token.Token = json.Token
	token.Secret = json.Secret

	client, err := consumer.MakeHttpClient(token)

	if err != nil {
		Debug().Error(err.Error())
		return nil
	}
	return client
}
