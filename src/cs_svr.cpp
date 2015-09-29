#include "cs_svr.h"
#include <string.h>
#include "rpc_net.h"
#include "rpc_log.h"

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
    this->dsptch_http_request(get_uri(), req_body, rsp_body);

    rsp_head += "HTTP/1.1 200 OK\r\n";
    rsp_head += "Content-Type: text/xml\r\n";
    rsp_head += "Content-Length: ";
    char buf[32] = { 0 };
    sprintf(buf, "%lu", rsp_body.size());
    rsp_head += buf;
    rsp_head += "\r\n\r\n";

    this->set_response(rsp_head + rsp_body);

    this->set_state("write");
    this->set_io_type('o');
    m_svr->add_io_event(this);
}

void cs_event::process_default(const std::string &uri,
        const std::string &req_body, std::string &rsp_body) {

    RPC_WARNING("invalid request from client, uri=%s, ip=%s, port=%u", 
            uri.c_str(), get_ip().c_str(), get_port());

    ezxml_t root = ezxml_new("message");
    ezxml_set_txt(root, "invalid request");
    rsp_body = ezxml_toxml(root);

    ezxml_free(root);
}

void cs_event::dsptch_http_request(const std::string &uri, 
        const std::string &req_body, std::string &rsp_body) {

    if (uri.find("/add") == 0) {
        process_add(uri, req_body, rsp_body, true);
    }
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
