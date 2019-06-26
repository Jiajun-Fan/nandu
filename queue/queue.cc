#include "queue.hh"

Queue::Queue() : _id(0) {
    pthread_mutex_init(&_lock, NULL);
}

Queue::~Queue() {
    pthread_mutex_destroy(&_lock);
}

void Queue::pushTask(uint32_t ts, Task* task) {
    pthread_mutex_lock(&_lock);
    uint64_t key = ts << 32 + _id;
    _id++;
    _tasks[key] = std::unique_ptr<Task>(task);
    pthread_mutex_unlock(&_lock);
}

std::unique_ptr<Task> Queue::popTask() {
    pthread_mutex_lock(&_lock);
    if (_tasks.size() == 0) {
        return std::unique_ptr<Task>(new Task("sleep"));
    } else {
        auto ret = std::move(_tasks.begin()->second);
        _tasks.erase(_tasks.begin());
    }
    pthread_mutex_unlock(&_lock);
}
