#include "queue.hh"

Queue::Queue() : _id(0) {
    pthread_mutex_init(&_lock, NULL);
}

Queue::~Queue() {
    pthread_mutex_destroy(&_lock);
}

void Queue::pushTask(uint32_t ts, const Task& task) {
    pthread_mutex_lock(&_lock);
    uint64_t key = ts << 32 + _id;
    _id++;
    _tasks[key] = task;
    pthread_mutex_unlock(&_lock);
}

Task Queue::popTask() {
    pthread_mutex_lock(&_lock);
    if (_tasks.size() == 0) {
        pthread_mutex_unlock(&_lock);
        Task sleep("sleep");
        sleep.addParam("1");
        return sleep;
    } else {
        Task ret(_tasks.begin()->second);
        _tasks.erase(_tasks.begin());
        pthread_mutex_unlock(&_lock);
        return ret;
    }
}
