#pragma once

#include <pthread.h>
#include <map>
#include <stdint.h>
#include "task.hh"

class Queue {
public:
    Queue();
    virtual ~Queue();

    void pushTask(uint32_t ts, const Task& task);
    Task popTask();

private:
    std::map<uint64_t, Task >                       _tasks;
    uint32_t                                        _id;
    pthread_mutex_t                                 _lock;
};

