#pragma once

#include <pthread.h>
#include <map>
#include <memory>
#include <stdint.h>
#include "task.hh"

class Queue {
public:
    Queue();
    virtual ~Queue();

    void pushTask(uint32_t ts, Task* task);
    std::unique_ptr<Task> popTask();

private:
    std::map<uint64_t, std::unique_ptr<Task> >      _tasks;
    uint32_t                                        _id;
    pthread_mutex_t                                 _lock;
};

