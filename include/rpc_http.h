#ifndef __RPC_HTTP_H__
#define __RPC_HTTP_H__

#include <string>

int http_send(int fd, int timeout_ms,
        const std::string &head, const std::string &body);

int http_recv(int fd, int timeout_ms,
        std::string &head, std::string &body);

#endif
