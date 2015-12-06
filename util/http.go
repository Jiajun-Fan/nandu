package util

import (
	"encoding/json"
	"net/http"
)

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
