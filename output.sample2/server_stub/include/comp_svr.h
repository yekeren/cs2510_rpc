#ifndef __comp_svr_h__
#define __comp_svr_h__

#include "svr_base.h"
#include "http_event.h"

class comp_event: public http_event {
    public:
        comp_event(svr_base *svr);

    public:
        virtual void on_process();

        void dsptch_http_request(const std::string &uri, 
                const std::string &req_body, std::string &rsp_head, std::string &rsp_body);

        void do_default(const std::string &uri,
                const std::string &req_body, std::string &rsp_head, std::string &rsp_body);

        void do_get_svr_id(const std::string &req_body, 
                std::string &rsp_head, std::string &rsp_body);

    public:
        void process_test1_divide(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);

        void process_test1_transpose(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);

        void process_test1_sort(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);

        void process_test1_wordcount(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);

};

class comp_svr: public svr_base {
    public:
        comp_svr();

        virtual ~comp_svr();

    public:
        virtual io_event *create_event(int fd,
                const std::string &ip, unsigned short port);
};

#endif
