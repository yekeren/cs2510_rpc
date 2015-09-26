#ifndef __CS_SVC_H__
#define __CS_SVC_H__

#include <string>
#include <vector>

struct svr_inst_t {
    int id;
    std::string name;
    std::string version;
    std::string ip;
    unsigned short port;
};

class ds_svc {
    protected:
        ds_svc();
        virtual ~ds_svc();

    public:
        static ds_svc *instance();

        static void destroy();

    private:
        int get_ips_by_host(const std::string &host_name,
                std::vector<std::string> &ips_list);

    public:
        int get_instances_by_name(const std::string &name,
                std::vector<svr_inst_t> &svr_instants_list);

    private:
        static ds_svc *m_pthis;
};

#endif
