package core

type Task interface {
	Init(uint64)
	Run() error

	GetID() uint64
	Name() string

	Diag() string
}

type TaskBase struct {
	id   uint64
	name string
}

func (task *TaskBase) Run() error {
	return nil
}

func (task *TaskBase) Init(id uint64) {
	task.id = id
}

func (task *TaskBase) GetID() uint64 {
	return task.id
}

func (task *TaskBase) Name() string {
	return task.name
}

func (task *TaskBase) Diag() string {
	return ""
}
