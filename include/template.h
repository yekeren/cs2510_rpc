#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include <string>

/**
 * @brief generate http head for specific http_code and content_len
 *
 * @param http_code
 * @param content_len
 *
 * @return 
 */
std::string gen_http_head(const std::string &http_code, int content_len);

#endif
