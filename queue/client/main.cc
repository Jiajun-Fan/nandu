#include "ND_client.hh"
#include "ND_task.hh"
#include "log.hh"

int main() {
    SetLogLevel(kLogDebug);
    NanduClient client("127.0.0.1", 6161, "password");
    NanduTask task1(Task("fuck"));
    NanduTask task2(Task("whatever"));

    task1.addParam("-param1");
    task1.addParam("-param2");

    client.push(task1);
    client.pop(task2);
    client.pop(task2);
    return 0;
}
