package nandu

type Task interface {
	Run(q *Q) []Task
}

func NewTasks(json *JsonTasks) []Task {
	if json.Type == "tumblrpage" {
		tasks := NewTumblrPages(json)
		return tasks
	} else {
		Debug().Error("undefined task type\n")
		return nil
	}
}
