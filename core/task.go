package core

const (
	kTaskNew = iota
	kTaskQueued
	kTaskRunning
	kTaskSuccess
	kTaskFailed
)

type Task interface {
	Init(uint64)
	Run() error
	Finish(bool)

	GetID() uint64
	Name() string
	Status() int

	Diag() string
}

type TaskBase struct {
	id     uint64
	name   string
	status int
}

func (task *TaskBase) Run() error {
	task.status = kTaskRunning
	return nil
}

func (task *TaskBase) Init(id uint64) {
	task.id = id
	task.status = kTaskQueued
}

func (task *TaskBase) Finish(success bool) {
	if success {
		task.status = kTaskSuccess
	} else {
		task.status = kTaskFailed
	}
}

func (task *TaskBase) GetID() uint64 {
	return task.id
}

func (task *TaskBase) Name() string {
	return task.name
}

func (task *TaskBase) Status() int {
	return task.status
}

func (task *TaskBase) Diag() string {
	return ""
}
