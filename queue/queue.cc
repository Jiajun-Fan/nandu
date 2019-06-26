#include "queue.hh"

void Queue::addTask(uint32_t ts, Task* task) {
    uint64_t key = ts << 32 + _id;
    _id++;
    _tasks[key] = std::unique_ptr<Task>(task);
}

int main() {
    Task* t = new Task();
    IntParam* i1 = new IntParam(1);
    IntParam* i2 = new IntParam(2);
    IntParam* i3 = new IntParam(3);
    t->addParam(i1);
    t->addParam(i2);
    t->addParam(i3);
    delete t;
    return 0;
}
