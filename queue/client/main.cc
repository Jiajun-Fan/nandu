#include "task_client.hh"
#include "packaged_task.hh"
#include "log.hh"

int main() {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    TaskClient client("127.0.0.1", 6161, "password");
    PackagedTask task1(Task("fuck"));
    PackagedTask task2(Task("whatever"));

    task1.addParam("-param1");
    task1.addParam("-param2");

    client.push(task1);
    client.pop(task2);
    client.pop(task2);
    return 0;
}
