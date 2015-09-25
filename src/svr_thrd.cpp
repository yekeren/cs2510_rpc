#include "svr_thrd.h"
#include <pthread.h>
#include "rpc_log.h"
#include "rpc_net.h"
#include "rpc_http.h"

/**************************************
* svr_thrd_base
**************************************/

svr_thrd_base::svr_thrd_base(): m_fd(0), m_port(0) {
}

svr_thrd_base::~svr_thrd_base() {
}

void *svr_thrd_base::run_routine(void *args) {
    svr_thrd_base *pthis = (svr_thrd_base*)args;
    pthis->run_routine();
    return NULL;
}

void svr_thrd_base::run_routine() {
    RPC_DEBUG("serving..., fd=%d, ip=%s, port=%u", m_fd, m_ip.c_str(), m_port);

    std::string head, body;
    int ret = http_recv(m_fd, 100 * 1000, head, body);

    ret = http_send(m_fd, 100 * 1000, "202\r\n\r\n", "hello, world");

    close(m_fd);
    delete this; /* ugly impl */
}

int svr_thrd_base::run() {
    pthread_t thrd_id;
    int ret = pthread_create(&thrd_id, NULL, run_routine, this);
    if (0 != ret) {
        RPC_WARNING("pthread_create() error, errno=%d", errno);
        return -1;
    }
    RPC_DEBUG("pthread_create() succ");
    return 0;
}

void svr_thrd_base::proc_new_conn(int fd, 
        const std::string &ip, uint16_t port) {
    m_fd = fd;
    m_ip = ip;
    m_port = port;
}

/**************************************
* svr_thrd_mgr_base
**************************************/
svr_thrd_mgr_base::svr_thrd_mgr_base() {
}

svr_thrd_mgr_base::~svr_thrd_mgr_base() {
}

void svr_thrd_mgr_base::proc_new_conn(
        int fd, const std::string &ip, uint16_t port) {

    svr_thrd_base *thrd = new svr_thrd_base;
    thrd->proc_new_conn(fd, ip, port);
    thrd->run();
}
