#ifndef __DS_SVR_H__
#define __DS_SVR_H__

#include <map>
#include <unordered_map>
#include "http_event.h"
#include "common_def.h"
#include "ezxml.h"
#include "svr_base.h"

/* svr_id -> <svr_inst_t, check_time> */
typedef std::map<std::string, std::pair<svr_inst_t, unsigned long long> > svr_insts_map_t;

/* svc_name -> svr_insts_map_t */
typedef std::unordered_map<std::string, svr_insts_map_t > svc_map_t;

class cs_event: public http_event {
    public:
        /**
         * @brief construct
         *
         * @param svr
         */
        cs_event(svr_base *svr);

    public:
        /**
         * @brief notify process event(multi-threads call)
         */
        virtual void on_process();

        /**
         * @brief route http request to spesific function
         *
         * @param uri
         * @param req_body
         * @param rsp_body
         */
        void dsptch_http_request(const std::string &uri, 
                const std::string &req_body, std::string &rsp_body);

    private:
        /**
         * @brief default process
         *
         * @param uri
         * @param req_body
         * @param rsp_body
         */
        void process_default(const std::string &uri,
                const std::string &req_body, std::string &rsp_body);

        /**
         * @brief process register request
         *
         * @param uri
         * @param req_body
         * @param rsp_body
         * @param flag: true for register and false for unregister
         */
        void process_register(const std::string &uri,
                const std::string &req_body, std::string &rsp_body, bool flag);

        /**
         * @brief process get_insts_by_name request
         *
         * @param uri
         * @param req_body
         * @param rsp_body
         */
        void process_get_insts_by_name(const std::string &uri,
                const std::string &req_body, std::string &rsp_body);
};

class cs_svr: public svr_base {
    public:
        /**
         * @brief construct
         */
        cs_svr();

        /**
         * @brief destruct
         */
        virtual ~cs_svr();

    public:
        /**
         * @brief create event
         *
         * @param fd
         * @param ip
         * @param port
         *
         * @return 
         */
        virtual io_event *create_event(int fd,
                const std::string &ip, unsigned short port);

    public:
        /**
         * @brief register svr
         *
         * @param svr
         */
        void do_register(svr_inst_t &svr);

        /**
         * @brief unregister svr
         *
         * @param svr
         */
        void do_unregister(svr_inst_t &svr);

        /**
         * @brief get server insts by name of service
         *
         * @param name
         * @param svr_insts_list
         */
        void do_get_insts_by_name(const std::string &name,
                std::vector<svr_inst_t> &svr_insts_list);

    private:
        spin_lock m_lock;
        svc_map_t m_svc_map;
};

#endif
