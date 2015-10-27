#ifndef __cs_svr_h__
#define __cs_svr_h__

#include "svr_base.h"
#include "http_event.h"

class cs_event: public http_event {
    public:
        cs_event(svr_base *svr);

    public:
        virtual void on_process();

        void dsptch_http_request(const std::string &uri, 
                const std::string &req_body, std::string &rsp_head, std::string &rsp_body);

        void do_default(const std::string &uri,
                const std::string &req_body, std::string &rsp_head, std::string &rsp_body);

        void do_get_svr_id(const std::string &req_body, 
                std::string &rsp_head, std::string &rsp_body);

    public:
        void process_multiply(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);

        void process_max(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);

        void process_add(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);

        void process_wc(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);

};

class cs_svr: public svr_base {
    public:
        cs_svr();

        virtual ~cs_svr();

    public:
        virtual io_event *create_event(int fd,
                const std::string &ip, unsigned short port);
};

#endif
