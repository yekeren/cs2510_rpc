#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include <string>

/**
 * @brief generate http head with specific http_code and content_len
 *
 * @param http_code
 * @param content_len
 *
 * @return 
 */
std::string gen_http_head(
        const std::string &http_code, int content_len);

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
        const std::string &host, int content_len);

#endif
