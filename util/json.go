package util

import (
	"encoding/json"
	"io/ioutil"
)

func ReadJsonFile(file string, v interface{}) error {
	bytes, err := ioutil.ReadFile(file)
	if err != nil {
		return err
	}

	return json.Unmarshal(bytes, v)
}
