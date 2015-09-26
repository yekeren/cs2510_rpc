#include "rpc_http.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "rpc_net.h"
#include "rpc_log.h"

/**
 * @brief http talk
 *
 * @param ips_list available ips of the service
 * @param port service port
 * @param req_head
 * @param req_body
 * @param rsp_head
 * @param rsp_body
 * @param conn_timeout_ms
 * @param send_timeout_ms
 * @param recv_timeout_ms
 *
 * @return 
 */
int http_talk(const std::vector<std::string> &ips_list, unsigned short port,
        const std::string &req_head, const std::string &req_body,
        std::string &rsp_head, std::string &rsp_body,
        int conn_timeout_ms, int send_timeout_ms, int recv_timeout_ms) {

    /* connect to remote server */
    int fd = -1;
    for (int i = 0; i < (int)ips_list.size(); ++i) {
        fd = connect_ex((char*)ips_list[i].c_str(), port, conn_timeout_ms);
        if (-1 == fd) {
            RPC_WARNING("connect_ex() failed, fd=%d, ip=%s, port=%u", 
                    fd, ips_list[i].c_str(), port);
        } else {
            break;
        }
    }

    /* send data to the remote server */
    int send_len = send_ex(fd, (char*)req_head.c_str(), 
            req_head.length(), 0, send_timeout_ms);
    if (send_len < 0) {
        RPC_WARNING("send_ex() error, fd=%d", fd);
        return -1;
    }

    /* recv response from remote server */
    char buf[60] = { 0 };
    int recv_len = recv_ex(fd, buf, sizeof(buf), 
            0, recv_timeout_ms);
    if (recv_len < 0) {
        RPC_WARNING("recv_ex() error, fd=%d", fd);
        return -1;
    }
    RPC_DEBUG("%s", buf);
    return 0;
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
    char *pos = strcasestr((char*)head.c_str(), "content-length");
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
