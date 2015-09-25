#include "rpc_http.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "rpc_net.h"
#include "rpc_log.h"

static unsigned long long get_cur_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int http_send(int fd, int timeout_ms,
        const std::string &head, const std::string &body) {

    int ret = send_ex(fd, (char*)head.data(), head.length(), 0, 100000);
    if (ret <= 0) {
        return -1;
    }
    ret = send_ex(fd, (char*)body.data(), body.length(), 0, 100000);
    if (ret <= 0) {
        return -1;
    }
    return 0;
}

int http_recv(int fd, int timeout_ms,
        std::string &head, std::string &body) {

    bool read_head_done = false;

    char buf[32] = { 0 };

    /* create read file descriptors set */
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    /* set timeout */
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;

    while (!read_head_done) {
        if (select(fd + 1, &rfds, NULL, NULL, &tv) <= 0) {
            /* time expired or error */
            RPC_WARNING("select() error or timeout, fd=%d, errno=%d", fd, errno);
            return -1;
        } else {
            /* handle I/O event */
            int len = recv(fd, buf, sizeof(buf), 0);
            if (len == 0) { 
                RPC_WARNING("client closed socket, fd=%d", fd);
                return 0;
            } else if (len < 0) { 
                RPC_WARNING("recv() error, fd=%d, errno=%d", fd, errno);
                return -1;
            } else {
                /* incoming data */
                /* TODO: fix bug */
                char *pos = strstr(buf, "\r\n\r\n");
                if (NULL == pos) {
                    /* continue read head */
                    head.append(buf, len);
                    RPC_DEBUG("recv %d", len);
                } else {
                    /* read head done */
                    head.append(buf, pos - buf);
                    read_head_done = true;

                    body.append(pos + 4, len - (pos - buf) - 4);
                }
            }
        }
        /* TODO: update timeout */
    }
    RPC_DEBUG("\nread head done, %s", head.c_str());

    int content_len = 0;
    char *pos = strcasestr(head.c_str(), "content-length");
    if (pos) {
        char *pos_colon = strcasestr(pos, ":");
        if (pos_colon) {
            content_len = atoi(pos_colon + 1);

            int recv_len = body.length();
            body.resize(content_len);

            /* TODO: update timeout */
            int ret = recv_ex(fd, (char*)body.data() + recv_len, content_len - recv_len, 0, 100 * 1000);
            if (ret <= 0) {
                /* client closed socket or error */
                return ret;
            }
        }
    }
    RPC_DEBUG("read body done, %s", body.c_str());
    return 0;
}
