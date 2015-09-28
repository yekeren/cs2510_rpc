#include <signal.h>
#include <assert.h>
#include "rpc_log.h"
#include "ds_svr.h"

static bool running = true;
static void signal_proc(int signo) {
    running = false;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_proc);
    signal(SIGTERM, signal_proc);

    /* initialize server */
    ds_svr *svr = new ds_svr;
    assert(0 == svr->run(2));
    assert(0 == svr->bind(8001));

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