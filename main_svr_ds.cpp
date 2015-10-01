#include <signal.h>
#include <getopt.h>
#include <assert.h>
#include "rpc_log.h"
#include "ds_svr.h"

static bool running = true;
static void signal_proc(int signo) {
    running = false;
}

static void usage(int argc, char *argv[]) {
    printf("Usage: %s [options]\n", argv[0]);
    printf("-h/--help:      show this help\n");
    printf("-t/--threads:   specify threads number\n");
    printf("-p/--port:      specify service port\n");
}

int main(int argc, char *argv[]) {
    int port = 0;
    int threads_num = 4;

    /* command line options */
    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h'},
            { "port", required_argument, 0, 'p'},
            { "threads", required_argument, 0, 't'},
            { 0, 0, 0, 0 }
        };
        int option_index = 0;
        int c = getopt_long(argc, argv, "ht:p:", long_options, &option_index);
        if (-1 == c) {
            break;
        }
        switch (c) {
            case 'h':
                usage(argc, argv);
                exit(0);
            case 't':
                threads_num = atoi(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                usage(argc, argv);
                exit(0);
        }
    }
    if (0 == port) {
        usage(argc, argv);
        exit(0);
    }

    /* start service */
    RPC_WARNING("threads num=%d", threads_num);
    RPC_WARNING("listen port=%d", port);

    signal(SIGINT, signal_proc);
    signal(SIGTERM, signal_proc);

    /* initialize server */
    ds_svr *svr = new ds_svr;
    assert(0 == svr->run(threads_num));
    assert(0 == svr->bind(port));

    while (running) {
        svr->run_routine(10);
    }

    svr->stop();
    svr->join();

    /* delete */
    delete svr;

    RPC_WARNING("server exit");
    return 0;
}
