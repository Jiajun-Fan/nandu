#include "ND_task.hh"

ReasonCode NanduTask::toPackage() {
    return this->package(*this);
}

ReasonCode NanduTask::toTask() {
    CreateTaskFromPackage(*this, *this);
    return RC_OK;
}
