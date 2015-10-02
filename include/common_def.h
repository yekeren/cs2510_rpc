#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#define DIR_SVR_HOST "127.0.0.1"
#define DIR_SVR_PORT 8000

#define DIR_SVR_CONN_TIMEOUT 4000
#define DIR_SVR_SEND_TIMEOUT 4000
#define DIR_SVR_RECV_TIMEOUT 4000

#define HTTP_SEND_TIMEOUT 2000
#define HTTP_RECV_TIMEOUT 2000

struct svr_inst_t {
    std::string id;
    std::string name;
    std::string version;
    std::string ip;
    unsigned short port;
};

#endif
