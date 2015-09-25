#include "svr_base.h"
#include "rpc_log.h"
#include "rpc_net.h"

/**************************************
* svr_base
**************************************/
svr_base::svr_base() : m_fd(-1), m_thrd_mgr(NULL) {
}

svr_base::~svr_base() {
    if (m_fd >= 0) {
        RPC_DEBUG("closing svr socket, fd=%d", m_fd);
        close(m_fd);
    }
    m_fd = -1;
}

int svr_base::bind(uint16_t port, int backlog) {

    /* create socket */
    m_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == m_fd) {
        RPC_WARNING("socket() error, errno=%d", errno);
        return -1;
    }
    RPC_DEBUG("svr socket created, fd=%d", m_fd);

    /* set to nonblock socket */
    if (-1 == set_nonblock(m_fd)) {
        RPC_WARNING("set_nonblock() error, errno=%d", errno);
        return -1;
    }

    /* set reuseaddr */
    int val = 1;
    int ret = setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (-1 == ret) {
        RPC_WARNING("setsockopt() error, errno=%d", errno);
        return -1;
    }

    /* bind to port */
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    ret = ::bind(m_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (-1 == ret) {
        RPC_WARNING("bind() error, errno=%d", errno);
        return -1;
    }
    RPC_DEBUG("svr socket binded, port=%u", port);

    /* listen */
    ret = listen(m_fd, backlog);
    if (-1 == ret) {
        RPC_WARNING("listen() error, errno=%d", errno);
        return -1;
    }
    RPC_DEBUG("svr socket is listening, backlog=%d", backlog);

    RPC_INFO("svr binded succ, fd=%d, port=%u, backlog=%d", m_fd, port, backlog);
    return 0;
}

void svr_base::run_routine(int timeout_ms) {
    /* create read file descriptors set */
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(m_fd, &rfds);

    /* set timeout */
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;

    /* examine the I/O descriptors sets */
    if (select(m_fd + 1, &rfds, NULL, NULL, &tv) > 0) {

        struct sockaddr_in remote_addr;
        socklen_t remote_addr_len = sizeof(remote_addr);
        memset(&remote_addr, 0, sizeof(remote_addr));

        int fd = accept(m_fd, (struct sockaddr*)&remote_addr, &remote_addr_len);

        if (-1 == fd) {
            RPC_WARNING("accept() error, errno=%d", errno);
        } else {
            /* extract ip and port from remote_addr */
            char ip[32] = { 0 };
            uint16_t port = ntohs(remote_addr.sin_port);
            strcpy(ip, inet_ntoa(remote_addr.sin_addr));
            RPC_DEBUG("incoming connection, fd=%d, ip=%s, port=%u", fd, ip, port);

            /* set fd to nonblock */
            if (-1 == set_nonblock(fd)) {
                RPC_WARNING("set_nonblock() error, errno=%d", errno);
            }

            /* dispatch connection to specific thread */
            m_thrd_mgr->proc_new_conn(fd, ip, port);
        }
    }
}
