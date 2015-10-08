#include "template.h"
#include <stdio.h>

/**
 * @brief generate http head with specific http_code and content_len
 *
 * @param http_code
 * @param content_len
 *
 * @return 
 */
std::string gen_http_head(
        const std::string &http_code, int content_len) {

    char buf[512] = { 0 };
    sprintf(buf, "HTTP/1.1 %s\r\nContent-Type: text/xml\r\nContent-Length: %d\r\n\r\n", 
            http_code.c_str(), content_len);

    return buf;
}

/**
 * @brief generate http head with specific uri, host, and content_len
 *
 * @param uri
 * @param host
 * @param content_len
 *
 * @return 
 */
std::string gen_http_head(const std::string &uri, 
        const std::string &host, int content_len) {
    char buf[512] = { 0 };
    sprintf(buf, "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %d\r\n\r\n", 
            uri.c_str(), host.c_str(), content_len);

    return buf;
}
