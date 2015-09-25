#include "rpc_net.h"
#include "rpc_log.h"

int set_nonblock(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if (-1 == flag) {
        RPC_WARNING("fcntl() getfl error, errno=%d", errno);
        return -1;
    }
    int ret = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    if (-1 == ret) {
        RPC_WARNING("fcntl() setfl error, errno=%d", errno);
        return -1;
    }
    return 0;
}

int send_ex(int fd, char *buf, int expect_len,
        int flags, int timeout_ms) {
    /* create write file descriptors set */
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);

    /* set timeout */
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;

    int send_len = 0;
    while (send_len < expect_len) {
        if (select(fd + 1, NULL, &wfds, NULL, &tv) <= 0) {
            /* time expired or error */
            RPC_WARNING("select() error or timeout, fd=%d, errno=%d", fd, errno);
            return -1;
        } else {
            /* handle I/O event */
            int len = send(fd, buf + send_len, expect_len - send_len, flags);
            if (len <= 0) { 
                RPC_WARNING("send() error, fd=%d, errno=%d", fd, errno);
                return -1;
            } else {
                /* outcoming data */
                send_len += len;
            }
        }
        /* TODO: update timeout */
    }
    return send_len;
}

int recv_ex(int fd, char *buf, int expect_len, 
        int flags, int timeout_ms) {

    /* create read file descriptors set */
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    /* set timeout */
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;

    int recv_len = 0;
    while (recv_len < expect_len) {
        if (select(fd + 1, &rfds, NULL, NULL, &tv) <= 0) {
            /* time expired or error */
            RPC_WARNING("select() error or timeout, fd=%d, errno=%d", fd, errno);
            return -1;
        } else {
            /* handle I/O event */
            int len = recv(fd, buf + recv_len, expect_len - recv_len, flags);
            if (len == 0) { 
                RPC_WARNING("client closed socket, fd=%d", fd);
                return 0;
            } else if (len < 0) { 
                RPC_WARNING("recv() error, fd=%d, errno=%d", fd, errno);
                return -1;
            } else {
                /* incoming data */
                recv_len += len;
            }
        }
        /* TODO: update timeout */
    }
    return recv_len;
}
