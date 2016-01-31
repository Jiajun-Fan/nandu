package common

import (
	"bytes"
	"encoding/json"
	"fmt"
)

type Task struct {
	Id      uint64          `json:"id"`
	Project string          `json:"project"`
	TaskSet string          `json:"task_set"`
	Url     string          `json:"url"`
	Token   string          `json:"token"`
	Data    json.RawMessage `json:"data"`
}

func (task *Task) GetData(data interface{}) error {
	d := json.NewDecoder(bytes.NewBuffer(task.Data))
	err := d.Decode(data)
	return err
}

func (task *Task) SetData(data interface{}) error {
	m, err := json.Marshal(data)
	task.Data = m
	return err
}

/*func (task *Task) FillData(data interface{}) {

	data_type := reflect.TypeOf(data)    // type of data
	data_value := reflect.New(data_type) // value of ptr to data
	data_elem := data_value.Elem()       // value of data
	field_num := data_type.NumField()

	data_map, ok := task.Data.(map[string]interface{})
	if !ok {
		task.Data = data_elem.Interface()
		return
	}

	for i := 0; i < field_num; i++ {
		field_struct := data_type.Field(i)
		field_name := field_struct.Name
		if json_tag_name := field_struct.Tag.Get("json"); json_tag_name != "" {
			field_name = json_tag_name
		}
		if v, ok := data_map[field_name]; ok {
			field_value := data_elem.Field(i)
			if field_value.CanSet() == false {
				continue
			}
			if value, ok := v.(string); ok && field_struct.Type.Kind() == reflect.String {
				field_value.SetString(value)
			}
			if value, ok := v.(float64); ok {
				v_kind := field_struct.Type.Kind()
				if reflect.Int <= v_kind && v_kind <= reflect.Int64 {
					field_value.SetInt(int64(value))
				} else if reflect.Uint <= v_kind && v_kind <= reflect.Uint64 {
					field_value.SetUint(uint64(value))
				} else if reflect.Float32 == v_kind && v_kind == reflect.Float64 {
					field_value.SetFloat(value)
				}
			}
		}
	}
	task.Data = data_elem.Interface()
}*/

func (t *Task) PushLog() string {
	return fmt.Sprintf("[ PUSH ]: project: %s, task_set: %s, url: %s\n", t.Project, t.TaskSet, t.Url)
}

func (t *Task) PopLog() string {
	return fmt.Sprintf("[ POP  ]: project: %s, task_set: %s, url: %s\n", t.Project, t.TaskSet, t.Url)
}
