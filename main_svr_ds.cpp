#include <signal.h>
#include <assert.h>
#include "rpc_log.h"
#include "ds_svr.h"

static bool running = true;
static void signal_proc(int signo) {
    running = false;
}

int main(int argc, char *argv[]) {

#ifdef _WIN32
       //define something for Windows (32-bit and 64-bit, this part is common)
       #ifdef _WIN64
          //define something for Windows (64-bit only)
       #endif
#elif __APPLE__
    RPC_DEBUG("apple");
        #include "TargetConditionals.h"
        #if TARGET_IPHONE_SIMULATOR
             // iOS Simulator
        #elif TARGET_OS_IPHONE
            // iOS device
        #elif TARGET_OS_MAC
            // Other kinds of Mac OS
        #else
            // Unsupported platform
        #endif
#elif __linux
    RPC_DEBUG("linux");
        // linux
#elif __unix // all unices not caught above
        // Unix
#elif __posix
        // POSIX
#endif
    exit(0);

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
