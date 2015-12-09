package nandu

import (
	"errors"
	"github.com/mrjones/oauth"
	"net/http"
)

func NewOauth(appKey string, appSecret string, token string, secret string) (*http.Client, error) {
	if token == "" || secret == "" || appKey == "" || appSecret == "" {
		return nil, errors.New("token can't be empty")
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
		return nil, err
	}
	return client, nil
}
