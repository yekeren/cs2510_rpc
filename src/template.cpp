#include "template.h"
#include <stdio.h>

/**
 * @brief generate http head for specific http_code and content_len
 *
 * @param http_code
 * @param content_len
 *
 * @return 
 */
std::string gen_http_head(const std::string &http_code, int content_len) {

    char buf[128] = { 0 };

    sprintf(buf, "HTTP/1.1 %s\r\n", http_code.c_str());

    std::string head(buf);
    head += "Content-Type: text/xml\r\n";
    head += "Content-Length: ";
    sprintf(buf, "%d", content_len);
    head += buf;
    head += "\r\n\r\n";

    return head;
}
