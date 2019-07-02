#include "ND_client.hh"
#include "ND_task.hh"
#include "log.hh"

int main() {
    SetLogLevel(kLogDebug);
    NanduClient client("127.0.0.1", 6161, "password");
    NanduTask task1(Task("fuck"));
    NanduTask task2(Task("whatever"));

    client.push(task1);
    Info("Push task %s\n", task1.getName().c_str());

    client.pop(task2);
    Info("Pop task %s\n", task2.getName().c_str());
    return 0;
}
