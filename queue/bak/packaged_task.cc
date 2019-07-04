#include "packaged_task.hh"

ReasonCode PackagedTask::toPackage() {
    return this->package(*this);
}

ReasonCode PackagedTask::toTask() {
    CreateTaskFromPackage(*this, *this);
    return RC_OK;
}
