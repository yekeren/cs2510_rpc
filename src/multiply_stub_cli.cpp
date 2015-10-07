#include "multiply_stub_cli.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "ds_svc.h"
#include "rpc_log.h"
#include "rpc_http.h"
#include "basic_proto.h"

void multiply(int **A, int A_row, int A_col,
        int **B, int B_row, int B_col,
        int **C, int C_row, int C_col) {

    ds_svc *ds_inst = ds_svc::instance();

    /* get svr insts from directory service */
    std::vector<svr_inst_t> svr_insts_list;
    int ret = ds_inst->get_insts_by_name("comp_svr", svr_insts_list);
    assert(0 == ret);

    for (int i = 0; i < (int)svr_insts_list.size(); ++i) {
        svr_inst_t &svr_inst = svr_insts_list[i];
        RPC_DEBUG("svr instant [%d], name=%s, version=%s, id=%s, ip=%s, port=%u",
                i, svr_inst.name.c_str(), svr_inst.version.c_str(), svr_inst.id.c_str(), 
                svr_inst.ip.c_str(), svr_inst.port);
    }

    /* choose a svr inst randomly */
    int index = rand() % svr_insts_list.size();
    svr_inst_t &svr_inst = svr_insts_list[index];
    RPC_INFO("choose svr instant, name=%s, version=%s, id=%s, ip=%s, port=%u",
            svr_inst.name.c_str(), svr_inst.version.c_str(), svr_inst.id.c_str(), 
            svr_inst.ip.c_str(), svr_inst.port);

    basic_proto ap;
    ap.add_matrix(A, A_row, A_col);
    ap.add_matrix(B, B_row, B_col);
    ap.add_matrix(C, C_row, C_col);

    std::string req_head;
    char tbuf[32] = { 0 };
    req_head += "POST /multiply HTTP/1.1\r\n";
    req_head += "Host: " + svr_inst.ip + "\r\n";
    req_head += "Content-Length: ";
    sprintf(tbuf, "%d", ap.get_buf_len());
    req_head += tbuf;
    req_head += "\r\n\r\n";

    std::string req_body(ap.get_buf(), ap.get_buf_len());

    /* request for computing */
    std::string rsp_head, rsp_body;
    
    int conn_timeout_ms = 2000;
    int send_timeout_ms = 2000;
    int recv_timeout_ms = 2000;
    
    std::vector<std::string> ips_list;
    ips_list.push_back(svr_inst.ip.c_str());
    
    http_talk(ips_list, svr_inst.port, req_head, req_body, rsp_head, rsp_body, conn_timeout_ms, send_timeout_ms, recv_timeout_ms);
    
    /* data unmarshalling */
    basic_proto ap_rsp(rsp_body.data(), rsp_body.size());
    return;

    //return retval;
}
