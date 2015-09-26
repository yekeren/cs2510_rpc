#ifndef __DS_SVC_H__
#define __DS_SVC_H__

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
        /**
         * @brief construct 
         */
        ds_svc();

        /**
         * @brief destruct
         */
        virtual ~ds_svc();

    public:
        /**
         * @brief create singleton
         *
         * @return 
         */
        static ds_svc *instance();

        /**
         * @brief destroy singleton
         */
        static void destroy();

    private:
        /**
         * @brief get ips from host name
         *
         * @param host_name
         * @param ips_list
         *
         * @return 
         */
        int get_ips_by_host(const std::string &host_name,
                std::vector<std::string> &ips_list);

    public:
        /**
         * @brief get svr instances by service name
         *
         * @param name
         * @param svr_instants_list
         *
         * @return 
         */
        int get_instances_by_name(const std::string &name,
                std::vector<svr_inst_t> &svr_instants_list);

    private:
        static ds_svc *m_pthis;
};

#endif
