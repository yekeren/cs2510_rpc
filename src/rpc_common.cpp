#include "rpc_common.h"
#include "ezxml.h"
#include "rpc_log.h"
#include "rpc_http.h"
#include "template.h"

/**
 * @brief 
 *
 * @param num
 *
 * @return 
 */
std::string num_to_str(int num) {
    char buf[1024] = { 0 };
    sprintf(buf, "%d", num);
    return buf;
}

/**
 * @brief locate server instances
 *
 * @param ip address of directory server
 * @param port end point of directory server
 * @param id
 * @param version
 * @param svr_insts_list
 *
 * @return 
 */
int get_insts_by_id(const std::string &ip, unsigned short port,
        int id, const std::string &version,
        std::vector<svr_inst_t> &svr_insts_list) {

    /* talk to directory server */
    std::string req_head, req_body;
    std::string rsp_head, rsp_body;

    char head_buf[1024] = { 0 };
    sprintf(head_buf, "GET /get_insts_by_id?id=%d&version=%s HTTP/1.1\r\nHost: %s\r\n\r\n", 
            id, version.c_str(), ip.c_str());

    req_head.assign(head_buf);
    int ret = http_talk(ip, port, req_head, req_body, rsp_head, rsp_body);
    if (0 > ret) {
        RPC_WARNING("http_talk() to directory server error");
        return -1;
    }

    /* parse xml data */
    ezxml_t root = ezxml_parse_str(
            (char*)rsp_body.c_str(), rsp_body.length());

    for (ezxml_t child = ezxml_child(root, "server"); child != NULL; child = child->next) {
        svr_inst_t svr_inst;
        svr_inst.id = atoi(ezxml_child(child, "id")->txt);
        svr_inst.name = ezxml_child(child, "name")->txt;
        svr_inst.version = ezxml_child(child, "version")->txt;
        svr_inst.ip = ezxml_child(child, "ip")->txt;
        svr_inst.port = atoi(ezxml_child(child, "port")->txt);

        svr_insts_list.push_back(svr_inst);
        RPC_DEBUG("list svr, id=%d, name=%s, version=%s,ip=%s, port=%u", 
                svr_inst.id, svr_inst.name.c_str(), svr_inst.version.c_str(),
                svr_inst.ip.c_str(), svr_inst.port);
    }
    ezxml_free(root);
    return 0;
}

/**
 * @brief get server information
 *
 * @param ip
 * @param port
 * @param id
 * @param version
 *
 * @return 
 */
int get_svr_id(const std::string &ip, unsigned short port,
        int &id, std::string &version) {

    /* talk to server */
    std::string req_head, req_body;
    std::string rsp_head, rsp_body;

    char head_buf[1024] = { 0 };
    sprintf(head_buf, "GET /get_svr_id HTTP/1.1\r\nHost: %s\r\n\r\n", ip.c_str());

    req_head.assign(head_buf);
    int ret = http_talk(ip, port, req_head, req_body, rsp_head, rsp_body);
    if (0 > ret) {
        RPC_WARNING("http_talk() to directory server error");
        return -1;
    }

    /* parse xml data */
    ezxml_t root = ezxml_parse_str(
            (char*)rsp_body.c_str(), rsp_body.length());

    id = atoi(ezxml_child(root, "id")->txt);
    version = ezxml_child(root, "version")->txt;

    RPC_DEBUG("get svr, id=%d, version=%s,ip=%s, port=%u", 
            id, version.c_str(), ip.c_str(), port);

    ezxml_free(root);
    return 0;
}

/**
 * @brief get and verify server
 *
 * @param ip
 * @param port
 * @param id
 * @param version
 * @param svr_inst
 *
 * @return 
 */
int get_and_verify_svr(const std::string &ip, unsigned short port,
        int id, const std::string &version, svr_inst_t &svr_inst) {

    int ret_val = 0;

    /* locate server from directory service */
    std::vector<svr_inst_t> svr_insts_list;
    ret_val = get_insts_by_id(ip, port, id, version, svr_insts_list);
    if (svr_insts_list.size() == 0) {
        RPC_WARNING("no server located, id=%d, version=%s", id, version.c_str());
        return -1;
    }
    svr_inst = svr_insts_list[rand() % svr_insts_list.size()];
    RPC_INFO("server located, id=%d, version=%s, ip=%s, port=%u",
            svr_inst.id, svr_inst.version.c_str(),
            svr_inst.ip.c_str(), svr_inst.port);

    /* verify server version */
    int remote_id = 0;
    std::string remote_version;
    ret_val = get_svr_id(svr_inst.ip, svr_inst.port, remote_id, remote_version);
    if (id != remote_id || version != remote_version) {
        RPC_WARNING("error in server version, id=%d, version=%s, expect_id=%d, expect_version=%s", 
                remote_id, remote_version.c_str(), id, version.c_str());
        return -1;
    }
    RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u",
            svr_inst.id, svr_inst.version.c_str(),
            svr_inst.ip.c_str(), svr_inst.port);

    return 0;
}

int rpc_call_by_id(int proc_id, std::string svr_inst_ip, short svr_inst_port, basic_proto &inpro, std::string &rsp_head, std::string &rsp_body)
{
    char id_request[1024] = {0};
    sprintf(id_request, "call_by_id?id=%d", proc_id);
    std::string req_head = gen_http_head(id_request, svr_inst_ip, inpro.get_buf_len());
    std::string req_body(inpro.get_buf(), inpro.get_buf_len());
    return http_talk(svr_inst_ip, svr_inst_port, req_head, req_body, rsp_head, rsp_body);
}
