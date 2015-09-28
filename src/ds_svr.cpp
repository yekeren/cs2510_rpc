#include "ds_svr.h"
#include "rpc_log.h"

/**
 * @brief construct
 *
 * @param svr
 */
ds_event::ds_event(svr_base *svr): http_event(svr) {
}

/**
 * @brief notify process event(multi-threads call)
 */
void ds_event::on_process() {

    /* get request */
    std::string req_head = get_head();
    std::string req_body = get_body();

    /* handle process */
    RPC_DEBUG("ip=%s, port=%u, method=%s, uri=%s, version=%s", 
            get_ip().c_str(), get_port(), 
            get_method().c_str(), get_uri().c_str(), get_version().c_str());

    std::string rsp_head, rsp_body;
    this->dsptch_http_request(get_uri(), req_body, rsp_body);

    /* set response */
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

/**
 * @brief default process
 *
 * @param uri
 * @param req_body
 * @param rsp_body
 */
void ds_event::process_default(const std::string &uri,
        const std::string &req_body, std::string &rsp_body) {

    RPC_WARNING("invalid request from client, uri=%s, ip=%s, port=%u", 
            uri.c_str(), get_ip().c_str(), get_port());

    ezxml_t root = ezxml_new("message");
    ezxml_set_txt(root, "invalid request");
    rsp_body = ezxml_toxml(root);

    ezxml_free(root);
}

/**
 * @brief process register request
 *
 * @param uri
 * @param req_body
 * @param rsp_body
 */
void ds_event::process_register(const std::string &uri,
        const std::string &req_body, std::string &rsp_body) {

    /* parse parameter in the xml */
    ezxml_t root = ezxml_parse_str(
            (char*)req_body.data(), req_body.size());
    if (NULL == root) {
        /* xml parse error */
        process_default(uri, req_body, rsp_body);
        return;
    } 

    svr_inst_t svr;
    if (ezxml_child(root, "id")) {
        svr.id = ezxml_child(root, "id")->txt;
    }
    if (ezxml_child(root, "name")) {
        svr.name = ezxml_child(root, "name")->txt;
    }
    if (ezxml_child(root, "version")) {
        svr.version = ezxml_child(root, "version")->txt;
    }
    if (ezxml_child(root, "ip")) {
        svr.ip = ezxml_child(root, "ip")->txt;
    }
    if (ezxml_child(root, "port")) {
        svr.port = atoi(ezxml_child(root, "port")->txt);
    }
    ezxml_free(root);

    /* succ */
    RPC_DEBUG("register succ, id=%s, name=%s, version=%s, ip=%s, port=%u", 
            svr.id.c_str(), svr.name.c_str(), svr.version.c_str(),
            svr.ip.c_str(), svr.port);

    root = ezxml_new("message");
    ezxml_set_txt(root, "succ");
    rsp_body = ezxml_toxml(root);
    ezxml_free(root);
}

/**
 * @brief process get_insts_by_name request
 *
 * @param uri
 * @param req_body
 * @param rsp_body
 */
void ds_event::process_get_insts_by_name(const std::string &uri,
        const std::string &req_body, std::string &rsp_body) {
    std::string name = uri.substr(strlen("/get_insts_by_name?name="));
    RPC_DEBUG("find insts by name, name=%s", name.c_str());
}

/**
 * @brief route http request to spesific function
 *
 * @param uri
 * @param req_body
 * @param rsp_body
 */
void ds_event::dsptch_http_request(const std::string &uri, 
        const std::string &req_body, std::string &rsp_body) {

    if (uri.find("/register") == 0) {
        process_register(uri, req_body, rsp_body);
    } else if (uri.find("/get_insts_by_name?name=") == 0) {
        process_get_insts_by_name(uri, req_body, rsp_body);
    } else {
        process_default(uri, req_body, rsp_body);
    }
}

/**
 * @brief create event
 *
 * @param fd
 * @param ip
 * @param port
 *
 * @return 
 */
io_event *ds_svr::create_event(int fd,
        const std::string &ip, unsigned short port) {

    ds_event *evt = new ds_event(this);
    evt->set_fd(fd);
    evt->set_ip(ip);
    evt->set_port(port);

    evt->set_io_type('i');
    evt->set_state("read_head");
    return (io_event*)evt;
}
