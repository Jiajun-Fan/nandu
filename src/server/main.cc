#include "server.hh"
#include "auth_service.hh"
#include "task_service.hh"
#include "log.hh"
#include <string.h>
#include <getopt.h>

static char* kPassword = (char*)"";
static int kBacklog = 10;
static int kPort = 6161;
static bool kLocalOnly = false;

static void printHelp() {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\tnandu_server [-b backlog] [-p <port>] [-k <password>] [-l] [-h]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-b:\tspecify the max connections server can handle in parallel\n");
    fprintf(stderr, "\t-p:\tspecify the port number\n");
    fprintf(stderr, "\t-k:\tspecify the passowrd\n");
    fprintf(stderr, "\t-l:\tserver is accessible only on localhost\n");
    fprintf(stderr, "\t-h:\tprint this help message\n");
}

static int parseArgs(int argc, char* const* argv) {
    int opt;
    int port = -1, backlog = -1;
    while ((opt=getopt(argc, argv, "b:p:k:lh")) > 0) {
        switch (opt) {
        case 'b':
            backlog = atoi(optarg);
            if (backlog < 0) {
                return -1;
            }
            kBacklog = backlog;
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
        case 'l':
            kLocalOnly = true;
            break;
        case 'h':
            return 0;
        default:
            return -1;
        }
    }
    return 1;
}

int main(int argc, char* const* argv) {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    int argsOK = parseArgs(argc, argv);
    if (argsOK <= 0) {
        printHelp();
        return argsOK;
    }
    Server* server = MakeServer(kLocalOnly, kPort, kBacklog);
    if (strlen(kPassword) > 0) {
        server->registerService(new AuthServerService(kPassword));
    }
    server->registerService(new TaskServerService());
    server->run();

    delete server;
    return 0;
}
