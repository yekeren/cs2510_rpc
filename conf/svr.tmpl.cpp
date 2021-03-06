#include "$name$.h"
#include "$name$_svr.h"
#include <string.h>
#include "template.h"
#include "rpc_net.h"
#include "rpc_log.h"
#include "rpc_common.h"
#include "ezxml.h"
#include "basic_proto.h"

/**************************************
 * $name$_event
 **************************************/
$name$_event::$name$_event(svr_base *svr): 
    http_event(svr) {
}

void $name$_event::on_process() {

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

void $name$_event::do_default(const std::string &uri,
        const std::string &req_body, std::string &rsp_head, std::string &rsp_body) {

    RPC_WARNING("invalid request from client, uri=%s, ip=%s, port=%u", 
            uri.c_str(), get_ip().c_str(), get_port());

    ezxml_t root = ezxml_new("message");
    ezxml_set_txt(root, "invalid request");
    rsp_body = ezxml_toxml(root);
    ezxml_free(root);

    rsp_head = gen_http_head("404 Not Found", rsp_body.size());
}

void $name$_event::dsptch_http_request(const std::string &uri, 
        const std::string &req_body, std::string &rsp_head, std::string &rsp_body) {
    if (uri.find("/get_svr_id") == 0) {
        do_get_svr_id(req_body, rsp_head, rsp_body);
    }
    else if (uri.find("/call_by_id?id=") == 0) {
        int id = atoi(uri.substr(strlen("/call_by_id?id=")).c_str());
        switch (id) {
            $dispatch$
            default:
                RPC_WARNING("invalid request %s", uri.c_str());
        }
    }
    else {
        RPC_WARNING("invalid request %s", uri.c_str());
    }
}

void $name$_event::do_get_svr_id(const std::string &req_body, 
        std::string &rsp_head, std::string &rsp_body) {

    ezxml_t root = ezxml_new("server");
    ezxml_set_txt(ezxml_add_child(root, "id", 0), num_to_str(RPC_ID).c_str());
    ezxml_set_txt(ezxml_add_child(root, "name", 0), RPC_NAME);
    ezxml_set_txt(ezxml_add_child(root, "version", 0), RPC_VERSION);

    rsp_body = ezxml_toxml(root);
    ezxml_free(root);

    rsp_head = gen_http_head("200 OK", rsp_body.size());
    rsp_body.assign(rsp_body.data(), rsp_body.size());
}

$stub$
    
/**************************************
 * $name$_svr
 **************************************/
$name$_svr::$name$_svr() {
}

$name$_svr::~$name$_svr() {
}

io_event *$name$_svr::create_event(int fd,
        const std::string &ip, unsigned short port) {

    $name$_event *evt = new $name$_event(this);
    evt->set_fd(fd);
    evt->set_ip(ip);
    evt->set_port(port);

    evt->set_io_type('i');
    evt->set_state("read_head");
    evt->set_timeout(RPC_RECV_TIMEOUT);
    return (io_event*)evt;
}
