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
    std::string ds_ip = "127.0.0.1";
    int ds_port = 8000;
    int port = 8001;

    signal(SIGINT, signal_proc);
    signal(SIGTERM, signal_proc);

    /* initialize server */
    cs_svr *svr = new cs_svr;
    assert(0 == svr->register_service(ds_ip, ds_port, "conf/cs.8001"));
    assert(0 == svr->run(2));
    assert(0 == svr->bind(port));

    while (running) {
        svr->run_routine(10);
    }

    svr->stop();
    svr->join();

    /* delete */
    svr->unregister_service(ds_ip, ds_port);
    delete svr;

    RPC_WARNING("server exit");
    return 0;
}
