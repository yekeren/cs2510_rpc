#include "ds_svc.h"
#include "common_def.h"
#include "rpc_net.h"
#include "rpc_http.h"
#include "rpc_log.h"
#include "ezxml.h"

ds_svc *ds_svc::m_pthis = NULL;

/**
 * @brief construct 
 */
ds_svc::ds_svc() {
}

/**
 * @brief destruct
 */
ds_svc::~ds_svc() {
}

/**
 * @brief create singleton
 *
 * @return 
 */
ds_svc *ds_svc::instance() {
    if (NULL == m_pthis) {
        m_pthis = new ds_svc;
    }
    return m_pthis;;
}

/**
 * @brief destroy singleton
 */
void ds_svc::destroy() {
    if (NULL != m_pthis) {
        delete m_pthis;
    }
    m_pthis = NULL;
}

/**
 * @brief get ips from host name
 *
 * @param host_name
 * @param ips_list
 *
 * @return 
 */
int ds_svc::get_ips_by_host(const std::string &host_name,
        std::vector<std::string> &ips_list) {

    /* getaddrinfo */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res0 = NULL;
    int error = getaddrinfo(DIR_SVR_HOST, "http", &hints, &res0);
    if (error) {
        /* not reached */
        RPC_WARNING("host name is not reach, host=%s, err=%s", 
                host_name.c_str(), gai_strerror(error));
        return -1;
    }

    for (struct addrinfo *res = res0; res; res = res->ai_next) {
        std::string ip(inet_ntoa(((struct sockaddr_in*)res->ai_addr)->sin_addr));
        ips_list.push_back(ip);

        RPC_DEBUG("resolve ip from host, ip=%s", ip.c_str());
    }
    freeaddrinfo(res0);

    if (0 == ips_list.size()) {
        RPC_WARNING("no ip address is found, host=%s", 
                host_name.c_str());
        return -1;
    }
    return 0;
}

/**
 * @brief get svr insts by service name
 *
 * @param name
 * @param svr_insts_list
 *
 * @return 
 */
int ds_svc::get_insts_by_name(const std::string &name,
        std::vector<svr_inst_t> &svr_insts_list) {

    /* get ips_list by host name */
    std::vector<std::string> ips_list;
    if (0 > get_ips_by_host(DIR_SVR_HOST, ips_list)) {
        RPC_WARNING("get_ip_by_host() error, host=%s", DIR_SVR_HOST);
        return -1;
    }

    /* talk to directory service */
    std::string req_head, req_body;
    std::string rsp_head, rsp_body;

    req_head += std::string("GET /server-config.xml?name=") + name + " HTTP/1.1\r\n";
    req_head += "Host: " DIR_SVR_HOST "\r\n";
    req_head += "\r\n\r\n";

    int ret = http_talk(ips_list, DIR_SVR_PORT, 
            req_head, req_body, 
            rsp_head, rsp_body,
            DIR_SVR_CONN_TIMEOUT, 
            DIR_SVR_SEND_TIMEOUT, 
            DIR_SVR_RECV_TIMEOUT);
    if (0 > ret) {
        RPC_WARNING("http_talk() failed");
        return -1;
    }

    RPC_DEBUG("head=%s", rsp_head.c_str());
    RPC_DEBUG("body=%s", rsp_body.c_str());
    RPC_DEBUG("body_len=%lu", rsp_body.size());

    /* parse xml data */
    ezxml_t root = ezxml_parse_str(
            (char*)rsp_body.c_str(), rsp_body.length());

    for (ezxml_t child = ezxml_child(root, "server"); child != NULL; child = child->next) {
        svr_inst_t svr_inst;
        svr_inst.id = ezxml_child(child, "id")->txt;
        svr_inst.name = ezxml_child(child, "name")->txt;
        svr_inst.version = ezxml_child(child, "version")->txt;
        svr_inst.ip = ezxml_child(child, "ip")->txt;
        svr_inst.port = atoi(ezxml_child(child, "port")->txt);

        svr_insts_list.push_back(svr_inst);
        RPC_DEBUG("list svr, id=%s, name=%s, version=%s,ip=%s, port=%u", 
                svr_inst.id.c_str(), svr_inst.name.c_str(), svr_inst.version.c_str(),
                svr_inst.ip.c_str(), svr_inst.port);
    }
    ezxml_free(root);
    return 0;
}
