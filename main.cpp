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

    /* create computing threads */
    int thrds_num = 1;
    std::vector<svr_thrd*> thrds_list;
    thrds_list.resize(thrds_num);
    for (int i = 0; i < thrds_list.size(); ++i) {
        thrds_list[i] = new svr_thrd;
        thrds_list[i]->run();
    }

    /* initialize threads dispatcher */
    svr_thrd_dsptch *dsptch = new svr_thrd_dsptch;;
    dsptch->set_thrds(thrds_list);

    /* initialize server */
    svr_base *svr = new svr_base;
    svr->set_thrd_dsptch(dsptch);
    assert(0 == svr->bind(8001));

    while (running) {
        svr->run_routine(100);
    }

    /* stop computing threads */
    for (int i = 0; i < thrds_list.size(); ++i) {
        thrds_list[i]->stop();
    }
    for (int i = 0; i < thrds_list.size(); ++i) {
        thrds_list[i]->join();
        delete thrds_list[i];
    }

    /* delete */
    delete svr;
    delete dsptch;

    RPC_WARNING("server exit");
    return 0;
}
