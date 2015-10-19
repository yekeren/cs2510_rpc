#ifndef __RPC_COMMON_H__
#define __RPC_COMMON_H__

#include <string>
#include <vector>
#include "basic_proto.h"

#define RPC_CONN_TIMEOUT 5000
#define RPC_RECV_TIMEOUT 5000
#define RPC_SEND_TIMEOUT 5000

struct svr_inst_t {
    int id;
    std::string name;
    std::string version;
    std::string ip;
    unsigned short port;
};

/**
 * @brief 
 *
 * @param num
 *
 * @return 
 */
std::string num_to_str(int num);

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
        std::vector<svr_inst_t> &svr_insts_list);

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
        int &id, std::string &version);

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
        int id, const std::string &version, svr_inst_t &svr_inst);

int rpc_call_by_id(int proc_id, std::string svr_inst_ip, short svr_inst_port, basic_proto &inpro, std::string &rsp_head, std::string &rsp_body);


#endif
