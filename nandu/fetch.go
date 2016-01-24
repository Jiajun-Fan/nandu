package nandu

import (
	"github.com/Jiajun-Fan/nandu/common"
	"github.com/Jiajun-Fan/nandu/util"
	"io/ioutil"
)

type FetcherHandler func(taskset *TaskSet, task *common.Task) []byte

func CommonFetcher(taskset *TaskSet, task *common.Task) []byte {
	r, err := taskset.client.Get(task.Url)
	if err != nil {
		util.Error("can't fetch url %s, %s\n", task.Url, err.Error())
		return nil
	}

	defer r.Body.Close()

	bytes, err := ioutil.ReadAll(r.Body)

	if err != nil {
		util.Error("can't get bytes from url %s, %s\n", task.Url, err.Error())
		return nil
	}

	if r.StatusCode/100 != 2 {
		util.Error("get wrong status code url %s, %d\n", task.Url, r.StatusCode)
		return nil
	}

	return bytes
}
