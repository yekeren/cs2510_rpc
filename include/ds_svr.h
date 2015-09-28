#ifndef __DS_SVR_H__
#define __DS_SVR_H__

#include "http_event.h"
#include "common_def.h"
#include "ezxml.h"
#include "svr_base.h"

class ds_event: public http_event {
    public:
        /**
         * @brief construct
         *
         * @param svr
         */
        ds_event(svr_base *svr);

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
         */
        void process_register(const std::string &uri,
                const std::string &req_body, std::string &rsp_body);

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

class ds_svr: public svr_base {
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
};

#endif
