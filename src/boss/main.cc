#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fstream>
#include "server.hh"
#include "auth_service.hh"
#include "task_service.hh"
#include "log.hh"

static char* kPassword = (char*)"";
static int kBacklog = 10;
static int kPort = 6161;
static bool kLocalOnly = false;

static int parseConfigFile(const char* config) {
    std::ifstream fin(config);
    while (!fin.eof()) {
        std::string key, value;
        fin >> key >> value;
        if (key.length() == 0 || key[0] == '#') {
            continue;
        }
        if (key == "backlog") {
            int backlog = -1;
            backlog = atoi(value.c_str());
            if (backlog <= 0) {
                return -1;
            }
            kBacklog = backlog;
        } else if (key == "port") {
            int port = -1;
            port = atoi(value.c_str());
            if (port < 0) {
                return -1;
            }
            kPort = port;
        } else if (key == "password") {
            if (value == "") {
                return -1;
            }
            kPassword = strdup(value.c_str());
        } else if (key == "localOnly") {
            kLocalOnly = true;
        } else {
            return -1;
        }
    }
    return 1;
}

static void printHelp() {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "     nandu_server [-b backlog] [-p <port>] [-k <password>] [-l] [-h]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "     -b          specify the max connections server can handle in parallel\n");
    fprintf(stderr, "     -p          specify the port number\n");
    fprintf(stderr, "     -k          specify the passowrd\n");
    fprintf(stderr, "     -l          server is accessible only on localhost\n");
    fprintf(stderr, "     -h          print this help message\n");
    fprintf(stderr, "Configration file options:\n");
    fprintf(stderr, "     backlog     specify the max connections server can handle in parallel\n");
    fprintf(stderr, "     port        specify the port number\n");
    fprintf(stderr, "     password    specify the passowrd\n");
    fprintf(stderr, "     threadNum   server is accessible only on localhost\n");
}

static int parseArgs(int argc, char* const* argv) {
    int opt;
    int port = -1, backlog = -1;
    while ((opt=getopt(argc, argv, "c:b:p:k:lh")) > 0) {
        switch (opt) {
        case 'c':
            if (access(optarg, F_OK)) {
                return -1;
            }
            if (parseConfigFile(optarg) < 0) {
                return -1;
            }
            break;
        case 'b':
            backlog = atoi(optarg);
            if (backlog <= 0) {
                return -1;
            }
            kBacklog = backlog;
            break;
        case 'p':
            port = atoi(optarg);
            if (port <= 0) {
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
