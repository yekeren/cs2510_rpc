#include <signal.h>
#include <assert.h>
#include <vector>
#include "rpc_log.h"
#include "svr_base.h"
#include "svr_thrd.h"

static bool running = true;

/**
 * @brief signal handler
 *
 * @param signo
 */
static void signal_proc(int signo) {
    running = false;
}

/**
 * @brief main
 *
 * @param argc
 * @param argv[]]
 *
 * @return 
 */
int main(int argc, char *argv[]) {
    signal(SIGINT, signal_proc);
    signal(SIGTERM, signal_proc);

    /* initialize server */
    svr_base *svr = new svr_base;
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
