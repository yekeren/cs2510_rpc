#include <signal.h>
#include <assert.h>
#include "rpc_log.h"
#include "cs_svr.h"

static bool running = true;
static void signal_proc(int signo) {
    running = false;
}

int main(int argc, char *argv[]) {
#ifdef __APPLE__
    RPC_WARNING("running under osx");
#elif __linux
    RPC_WARNING("running under linux");
#endif

    signal(SIGINT, signal_proc);
    signal(SIGTERM, signal_proc);

    /* initialize server */
    cs_svr *svr = new cs_svr;
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
