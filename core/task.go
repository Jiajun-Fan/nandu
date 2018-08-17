package core

type Task interface {
	Run() error
	SetID(TaskID)
	GetID() TaskID
}

type TaskID uint64

func (task *TaskID) SetID(id TaskID) {
	*task = id
}

func (task *TaskID) GetID() TaskID {
	return *task
}
