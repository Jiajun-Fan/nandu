#pragma once
#include "task.hh"
#include "reason_code.hh"
class PackagedTask : public Task, public Package {
public:
    explicit PackagedTask(const Package& package) : 
       Package(package) {
    } 
    explicit PackagedTask(const Task& task) :
        Task(task) {
    }
    virtual ~PackagedTask() {}
    ReasonCode toPackage();
    ReasonCode toTask();
};
