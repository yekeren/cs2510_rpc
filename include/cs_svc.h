#ifndef DS_SVC_H
#define DS_SVC_H

#include <stdInt.h>
#include <string>

struct svr_t {
    std::string ip;
    uint16_t port;
};

class ds_svc {
    protected:
        ds_svc();
        virtual ~ds_svc();

    public:
        static ds_svc *instance();
        static void destroy();

    public:
        int get_svr_by_id(const std::string &id, 
                const std::string &version,
                svr_t &svr_inst);
};

#endif
