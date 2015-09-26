#ifndef __RPC_HTTP_H__
#define __RPC_HTTP_H__

#include <string>
#include <vector>

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
        int conn_timeout_ms, int send_timeout_ms, int recv_timeout_ms);

int http_send(int fd, int timeout_ms,
        const std::string &head, const std::string &body);

int http_recv(int fd, int timeout_ms,
        std::string &head, std::string &body);

#endif
