#include <unistd.h>
#include <pthread.h>
#include "client.hh"
#include "auth_service.hh"
#include "task_service.hh"
#include "task.hh"
#include "log.hh"
#include "exception.hh"

// FIXME hostname works only with IP address
static char* kHost = (char*)"127.0.0.1";
static char* kPassword = (char*)"";
static int kPort = 6161;
static int kThreads = 3;

static void printHelp() {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\tnandu_client [-m host] [-p <port>] [-k <password>] [-t <threads>] [-h]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-m:\tspecify the host name\n");
    fprintf(stderr, "\t-p:\tspecify the port number\n");
    fprintf(stderr, "\t-k:\tspecify the passowrd\n");
    fprintf(stderr, "\t-t:\tspecify the number of threads\n");
    fprintf(stderr, "\t-h:\tprint this help message\n");
}

static int parseArgs(int argc, char* const* argv) {
    int opt;
    int port = -1, threads = -1;
    while ((opt=getopt(argc, argv, "m:p:k:t:h")) > 0) {
        switch (opt) {
        case 'm':
            kHost = strdup(optarg);
            break;
        case 'p':
            port = atoi(optarg);
            if (port < 0) {
                return -1;
            }
            kPort = port;
            break;
       case 'k':
            kPassword = strdup(optarg);
            break;
       case 't':
            threads = atoi(optarg);
            if (threads < 0) {
                return -1;
            }
            kThreads = threads;
            break;
       case 'h':
            return 0;
        default:
            return -1;
        }
    }
    return 1;
}

static void* runWorker(void* args) {
    Client client(kHost, kPort);
    if (strlen(kPassword) > 0) {
        client.registerService(new AuthClientService(kPassword));
    }
    client.registerService(new TaskClientService());
    Session session = { 0, SC_INIT, "" };
    try {
        client.start(session);
        while (1) {
            Operation pop;
            client.run(session, Operation(TaskEnum::kOpPop), pop);
        }
        client.end(session);
    } catch (Exception e) {
        if (session.fd != 0) {
            close(session.fd);
        }
        Error("%s\n", e.what());
    }

    return 0;
}

int main(int argc, char* const*argv) {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    int argsOK = parseArgs(argc, argv);
    if (argsOK <= 0) {
        printHelp();
        return argsOK;
    }

    pthread_t* tids = (pthread_t*)malloc(sizeof(pthread_t)*kThreads);
    for (int i=0; i<kThreads; i++) {
        pthread_create(tids+i, NULL, runWorker, NULL);
    }
    for (int i=0; i<kThreads; i++) {
        pthread_join(tids[i], NULL);
    }

    return 0;
}
