package nandu

import (
	"errors"
	"github.com/mrjones/oauth"
	"net/http"
)

func NewOauth(json *JsonOauth) (*http.Client, error) {
	if json.Token == "" || json.Secret == "" || json.AppKey == "" || json.AppSecret == "" {
		return nil, errors.New("token can't be empty")
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
		return nil, err
	}
	return client, nil
}
