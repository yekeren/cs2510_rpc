#include "svr_base.h"
#include <string.h>
#include "rpc_log.h"
#include "rpc_net.h"
#define TRUE 1
#define FALSE 0

/**************************************
 * svr_base
 **************************************/
svr_base::svr_base() : m_thrd_dsptch(NULL), m_acc_evt(NULL) {
}

svr_base::~svr_base() {
    if (NULL != m_acc_evt) {
        delete m_acc_evt;
    }
    m_acc_evt = NULL;
}

int svr_base::bind(uint16_t port, int backlog) {
    m_acc_evt = new accept_event(this);
    if (0 > m_acc_evt->bind(port, backlog)) {
        RPC_WARNING("accept event bind() error, port=%u, backlog=%d", 
                port, backlog);
        return -1;
    }
    this->add_io_event(m_acc_evt);
    return 0;
}

void svr_base::add_io_event(io_event* evt) {
    m_evts_appd.push_back(evt);
}

void svr_base::run_routine(int timeout_ms){
    /* merge list */
    std::list<io_event*>::iterator iter = m_evts_appd.begin();
    for (; iter != m_evts_appd.end(); ++iter) {
        m_evts.push_back(*iter);
    }
    m_evts_appd.clear();

    /* create rfds and wfds */
    fd_set rfds, wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    int max_fd = -1;
    iter = m_evts.begin();
    for (; iter != m_evts.end(); ++iter) {
        io_event *evt = *iter;
        if ('i' == evt->get_io_type()) {
            FD_SET(evt->get_fd(), &rfds);
        } else {
            FD_SET(evt->get_fd(), &wfds);
        }
        if (evt->get_fd() > max_fd) {
            max_fd = evt->get_fd();
        }
    }

    /*set timeout*/
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;

    /* wait for io event */
    int ret = select(1 + max_fd, &rfds, &wfds, NULL, &tv);
    if (ret < 0) {
        RPC_WARNING("select() error, errno=%d", errno);
        return;
    }
    if (ret == 0) {
        return;
    }

    /* run state machine */
    iter = m_evts.begin();
    while (iter != m_evts.end()) {
        io_event *evt = *iter;
        fd_set *fds = NULL;
        if ('i' == evt->get_io_type()) {
            fds = &rfds;
        } else {
            fds = &wfds;
        }
        if (FD_ISSET(evt->get_fd(), fds)) {
            iter = m_evts.erase(iter);
            evt->on_event();
        } else {
            ++iter;
        }
    }
}

void svr_base::on_dispatch_task(http_event *evt) {
    m_thrd_dsptch->dispatch_task(evt);
}
