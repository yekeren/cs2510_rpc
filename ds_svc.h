#ifndef CS_SVC_H
#define CS_SVC_H

#include <stdInt.h>
#include <string>

struct svr_t {
    std::string ip;
    uint16_t port;
};

class cs_svc {
    protected:
        cs_svc();
        virtual ~cs_svc();

    public:
        static cs_svc *instance();
        static void destroy();

    public:
        int get_response(const std::string &ip, 
                uint16_t port,
                void *req_buf, int req_buf_len,
                void *rsp_buf, int &rsp_buf_len);
};

#endif
