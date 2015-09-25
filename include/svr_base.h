#ifndef __SVR_BASE__
#define __SVR_BASE__

#include <stdInt.h>
#include <string>
#include "svr_thrd.h"

class svr_base {
    public:
        svr_base();
        virtual ~svr_base();

    public:
        void set_thrd_mgr(svr_thrd_mgr_base *mgr) { m_thrd_mgr = mgr; }

        int bind(uint16_t port, int backlog = 32);

        void run_routine(int timeout_ms = 100);

    private:
        int m_fd;
        svr_thrd_mgr_base *m_thrd_mgr;
};

#endif
