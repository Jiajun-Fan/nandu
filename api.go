package nandu

type API struct {
	Type    string   `json:"type"`
	Name    string   `json:"name"`
	Derived string   `json:"derived"`
	Roots   []string `json:"roots"`
}
