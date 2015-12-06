package util

import (
	"bytes"
	"encoding/json"
	"net/http"
)

// server side
func HttpRequestUnmarshalJSON(v interface{}, req *http.Request) error {
	decoder := json.NewDecoder(req.Body)
	return decoder.Decode(v)
}

func HttpRespondMarshalJSON(v interface{}, w http.ResponseWriter) error {
	var data []byte
	var err error
	if data, err = json.Marshal(v); err == nil {
		w.Header().Set("Content-Type", "application/json")
		_, err = w.Write(data)
	}
	return err
}

func HttpRespondText(s string, w http.ResponseWriter) error {
	w.Header().Set("Content-Type", "text/plain")
	_, err := w.Write([]byte(s))
	return err
}

// client side
func HttpPostJSON(url string, data interface{}) (*http.Response, error) {
	b, err := json.Marshal(data)
	if err != nil {
		return nil, err
	}
	return http.Post(url, "application/json", bytes.NewBuffer(b))
}

func HttpResponseUnmarshalJSON(v interface{}, req *http.Response) error {
	decoder := json.NewDecoder(req.Body)
	return decoder.Decode(v)
}
