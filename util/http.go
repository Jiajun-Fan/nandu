package util

import (
	"encoding/json"
	"net/http"
)

func HttpRequestUnmarshalJSON(v interface{}, req *http.Request) error {
	decoder := json.NewDecoder(req.Body)
	return decoder.Decode(v)
}
