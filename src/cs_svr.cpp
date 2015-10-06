#include "cs_svr.h"
#include <string.h>
#include "common_def.h"
#include "rpc_net.h"
#include "rpc_log.h"
#include "template.h"
#include "ezxml.h"
#include "add_proto.h"
#include "wc_proto.h"

cs_event::cs_event(svr_base *svr): 
    http_event(svr) {
}

void cs_event::on_process() {

    std::string req_head = get_head();
    std::string req_body = get_body();

    RPC_DEBUG("ip=%s, port=%u, method=%s, uri=%s, version=%s", 
            get_ip().c_str(), get_port(), 
            get_method().c_str(), get_uri().c_str(), get_version().c_str());

    std::string rsp_head, rsp_body;
    this->dsptch_http_request(get_uri(), req_body, rsp_head, rsp_body);

    /* set response */
    this->set_response(rsp_head + rsp_body);

    this->set_state("write");
    this->set_io_type('o');
    m_svr->add_io_event(this);
}

void cs_event::process_default(const std::string &uri,
        const std::string &req_body, std::string &rsp_head, std::string &rsp_body) {

    RPC_WARNING("invalid request from client, uri=%s, ip=%s, port=%u", 
            uri.c_str(), get_ip().c_str(), get_port());

    ezxml_t root = ezxml_new("message");
    ezxml_set_txt(root, "invalid request");
    rsp_body = ezxml_toxml(root);
    ezxml_free(root);

    rsp_head = gen_http_head("404 Not Found", rsp_body.size());
}

void cs_event::dsptch_http_request(const std::string &uri, 
        const std::string &req_body, std::string &rsp_head, std::string &rsp_body) {

    if (uri.find("/add") == 0) {
        process_add(req_body, rsp_head, rsp_body);
    }
    else if (uri.find("/wc")==0){
        process_wc(req_body, rsp_head, rsp_body);
    }
    else {
        process_default(uri, req_body, rsp_head, rsp_body);
    }
}

void cs_event::process_add(const std::string &req_body, 
        std::string &rsp_head, std::string &rsp_body) {
    add_proto ap;
    ap.decode(req_body.data(), req_body.size());

    int a = ap.get_a();
    int b = ap.get_b();
    ap.set_retval(a + b);

    const char *buf = ap.encode();
    int buf_len = ap.get_buf_len();

    rsp_head = gen_http_head("200 OK", buf_len);
    rsp_body.assign(buf, buf_len);
}

void cs_event::process_wc(const std::string &req_body,
        std::string &rsp_head, std::string & rsp_body){
    wc_proto ap;
    ap.decode(req_body.data(), req_body.size());
    
    int m_str_len = ap.get_str_len();
    std::string str = ap.get_str();
    ap.set_retval(5);
    
    const char *buf = ap.encode();
    int buf_len = ap.get_buf_len();
    
    rsp_head = gen_http_head("wc OK", buf_len);
    rsp_body.assign(buf, buf_len);
    
}

cs_svr::cs_svr() {
}

cs_svr::~cs_svr() {
}

io_event *cs_svr::create_event(int fd,
        const std::string &ip, unsigned short port) {

    cs_event *evt = new cs_event(this);
    evt->set_fd(fd);
    evt->set_ip(ip);
    evt->set_port(port);

    evt->set_io_type('i');
    evt->set_state("read_head");
    evt->set_timeout(HTTP_RECV_TIMEOUT);
    return (io_event*)evt;
}
