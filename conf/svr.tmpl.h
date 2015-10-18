#ifndef __$name$_svr_h__
#define __$name$_svr_h__

#include "svr_base.h"
#include "http_event.h"

class $name$_event: public http_event {
    public:
        $name$_event(svr_base *svr);

    public:
        virtual void on_process();

        void dsptch_http_request(const std::string &uri, 
                const std::string &req_body, std::string &rsp_head, std::string &rsp_body);

        void do_default(const std::string &uri,
                const std::string &req_body, std::string &rsp_head, std::string &rsp_body);

        void do_get_svr_id(const std::string &req_body, 
                std::string &rsp_head, std::string &rsp_body);

    public:
        void process_add(const std::string &req_body, 
                std::string &rsp_head, std::string &rsp_body);
    
        void process_multiply(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);
    
        void process_max(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);
    
        void process_wc(const std::string &req_body,
                std::string &rsp_head, std::string &rsp_body);
};

class $name$_svr: public svr_base {
    public:
        $name$_svr();

        virtual ~$name$_svr();

    public:
        virtual io_event *create_event(int fd,
                const std::string &ip, unsigned short port);
};

#endif
