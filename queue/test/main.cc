#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "server.hh"
#include "client.hh"
#include "auth.hh"
#include "log.hh"

static void* runServer(void* args)  {
    Server server(kRemoteServer, 6100);
    server.registerService(new AuthServerService("password"));
    server.run();
}

static void testAuthOK() {
    Client client("127.0.0.1", 6100);
    client.registerService(new AuthClientService("password"));
    Session session = { 0, C_INIT, "", false};
    client.start(session);
    client.end(session);
}

static void testAuthBAD() {
    Client client("127.0.0.1", 6100);
    client.registerService(new AuthClientService("bad"));
    Session session = { 0, C_INIT, "", false};
    client.start(session);
    client.end(session);
}

int main() {
    SetLogLevel(kLogInfo);
    pthread_t tid;
    pthread_create(&tid, NULL, runServer, NULL);
    sleep(1);

    testAuthOK();
    testAuthBAD();
    return 0;
}
