#include "cs_svr.h"
#include <string.h>
#include "rpc_net.h"
#include "rpc_log.h"

/**************************************
 * cs_event
 **************************************/

/**
 * @brief construct
 *
 * @param svr
 */
cs_event::cs_event(svr_base *svr): http_event(svr) {
}

/**
 * @brief notify process event(multi-threads call)
 */
void cs_event::on_process() {

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
void cs_event::process_default(const std::string &uri,
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
 * @param flag
 */
void cs_event::process_register(const std::string &uri,
        const std::string &req_body, std::string &rsp_body, bool flag) {

    /* parse parameter in the xml */
    ezxml_t root = ezxml_parse_str(
            (char*)req_body.data(), req_body.size());
    if (NULL == root) {
        /* xml parse error */
        process_default(uri, req_body, rsp_body);
        return;
    } 

    /* validation */
    if (!ezxml_child(root, "id")
            || !ezxml_child(root, "name")
            || !ezxml_child(root, "version")
            || !ezxml_child(root, "ip")
            || !ezxml_child(root, "port")) {

        ezxml_free(root);

        process_default(uri, req_body, rsp_body);
        RPC_WARNING("missing data in the register request");
        return;
    }

    /* register */
    svr_inst_t svr;

    svr.id = ezxml_child(root, "id")->txt;
    svr.name = ezxml_child(root, "name")->txt;
    svr.version = ezxml_child(root, "version")->txt;
    svr.ip = ezxml_child(root, "ip")->txt;
    svr.port = atoi(ezxml_child(root, "port")->txt);

    ezxml_free(root);

    /* succ */
    if (flag) {
        ((cs_svr*)m_svr)->do_register(svr);
    } else {
        ((cs_svr*)m_svr)->do_unregister(svr);
    }

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
void cs_event::process_get_insts_by_name(const std::string &uri,
        const std::string &req_body, std::string &rsp_body) {

    /* invoke get insts by name */
    std::string name = uri.substr(strlen("/get_insts_by_name?name="));
    std::vector<svr_inst_t> insts_list;
    ((cs_svr*)m_svr)->do_get_insts_by_name(name, insts_list);

    /* create response */
    ezxml_t root = ezxml_new("service");
    for (int i = 0; i < (int)insts_list.size(); ++i) {
        svr_inst_t &svr = insts_list[i];

        ezxml_t child = ezxml_add_child(root, "server", i);
        ezxml_set_txt(ezxml_add_child(child, "id", 0), svr.id.c_str());
        ezxml_set_txt(ezxml_add_child(child, "name", 0), svr.name.c_str());
        ezxml_set_txt(ezxml_add_child(child, "version", 0), svr.version.c_str());
        ezxml_set_txt(ezxml_add_child(child, "ip", 0), svr.ip.c_str());
        char buf[32] = { 0 };
        sprintf(buf, "%u", svr.port);
        ezxml_set_txt(ezxml_add_child(child, "port", 0), buf);
    }

    rsp_body = ezxml_toxml(root);
    ezxml_free(root);
}

/**
 * @brief route http request to spesific function
 *
 * @param uri
 * @param req_body
 * @param rsp_body
 */
void cs_event::dsptch_http_request(const std::string &uri, 
        const std::string &req_body, std::string &rsp_body) {

    if (uri.find("/register") == 0) {
        process_register(uri, req_body, rsp_body, true);
    } else if (uri.find("/unregister") == 0) {
        process_register(uri, req_body, rsp_body, false);
    } else if (uri.find("/get_insts_by_name?name=") == 0) {
        process_get_insts_by_name(uri, req_body, rsp_body);
    } else {
        process_default(uri, req_body, rsp_body);
    }
}

/**************************************
 * cs_svr
 **************************************/
/**
 * @brief construct
 */
cs_svr::cs_svr() {
    m_lock.init();
}

/**
 * @brief destruct
 */
cs_svr::~cs_svr() {
    m_lock.destroy();
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

/**
 * @brief register svr
 *
 * @param svr
 */
void cs_svr::do_register(svr_inst_t &svr) {
    /* insert server into the hash */
    m_lock.lock();
    if (0 == m_svc_map.count(svr.name)) {
        svr_insts_map_t insts_map;
        m_svc_map[svr.name] = insts_map;
    }
    svr_insts_map_t &insts_map = m_svc_map[svr.name];
    insts_map[svr.id] = std::pair<svr_inst_t, unsigned long long>(
            svr, get_cur_msec());
    m_lock.unlock();

    /* record log */
    RPC_INFO("register succ, id=%s, name=%s, version=%s, ip=%s, port=%u", 
            svr.id.c_str(), svr.name.c_str(), svr.version.c_str(),
            svr.ip.c_str(), svr.port);
}

/**
 * @brief unregister svr
 *
 * @param svr
 */
void cs_svr::do_unregister(svr_inst_t &svr) {
    /* remove server from the hash */
    m_lock.lock();
    if (m_svc_map.count(svr.name) > 0) {
        svr_insts_map_t &insts_map = m_svc_map[svr.name];
        insts_map.erase(svr.id);
    }
    m_lock.unlock();

    /* record log */
    RPC_INFO("unregister succ, id=%s, name=%s, version=%s, ip=%s, port=%u", 
            svr.id.c_str(), svr.name.c_str(), svr.version.c_str(),
            svr.ip.c_str(), svr.port);
}

/**
 * @brief get server insts by name of service
 *
 * @param name
 * @param svr_insts_list
 */
void cs_svr::do_get_insts_by_name(const std::string &name,
        std::vector<svr_inst_t> &svr_insts_list) {
    /* get insts by name */
    m_lock.lock();
    if (m_svc_map.count(name) > 0) {
        svr_insts_map_t &insts_map = m_svc_map[name];
        svr_insts_map_t::iterator iter;
        for (iter = insts_map.begin(); iter != insts_map.end(); ++iter) {
            svr_insts_list.push_back(iter->second.first);
            svr_inst_t &svr = iter->second.first;
            RPC_DEBUG("get insts by name, id=%s, name=%s, version=%s, ip=%s, port=%u", 
                    svr.id.c_str(), svr.name.c_str(), svr.version.c_str(),
                    svr.ip.c_str(), svr.port);
        }
    }
    m_lock.unlock();
}
