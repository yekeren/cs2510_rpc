#ifndef __SVR_BASE__
#define __SVR_BASE__

#include <string>
#include <list>
#include "svr_thrd.h"
#include "io_event.h"

class svr_base {
    public:
        svr_base();
        virtual ~svr_base();

    public:
        void set_thrd_dsptch(svr_thrd_dsptch *dsptch) { m_thrd_dsptch = dsptch; }

        int bind(unsigned short port, int backlog = 32);

        void run_routine(int timeout_ms = 100);

        void add_io_event(io_event* evt);

    public:
        void on_dispatch_task(http_event *evt);

    private:
        std::list<io_event*> m_evts;
        std::list<io_event*> m_evts_appd;

        svr_thrd_dsptch *m_thrd_dsptch;

        accept_event *m_acc_evt;
};

#endif
