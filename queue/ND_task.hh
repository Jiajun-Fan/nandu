#pragma once
#include "task.hh"
#include "reason_code.hh"
class NanduTask : public Task, public Package {
public:
    explicit NanduTask(const Package& package) : 
       Package(package) {
    } 
    explicit NanduTask(const Task& task) :
        Task(task) {
    }
    virtual ~NanduTask() {}
    ReasonCode toPackage();
    ReasonCode toTask();
};
