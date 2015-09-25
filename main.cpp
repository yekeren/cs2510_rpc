#include <signal.h>
#include <assert.h>
#include "rpc_log.h"
#include "svr_base.h"

static bool running = true;
static void signal_proc(int signo) {
    running = false;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_proc);
    signal(SIGTERM, signal_proc);

    svr_base svr;
    svr_thrd_mgr_base mgr;

    svr.set_thrd_mgr(&mgr);

    assert(0 == svr.bind(8001));

    while (running) {
        svr.run_routine(100);
    }

    RPC_WARNING("server exit");
    return 0;
}
