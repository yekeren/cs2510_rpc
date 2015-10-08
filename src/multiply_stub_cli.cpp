#include "multiply_stub_cli.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "ds_svc.h"
#include "rpc_log.h"
#include "rpc_http.h"
#include "basic_proto.h"
#include "template.h"

void multiply(int *A, int A_row, int A_col,
        int *B, int B_row, int B_col,
        int *C, int C_row, int C_col) {

    /* get svr insts from directory service */
    ds_svc *ds_inst = ds_svc::instance();
    std::vector<svr_inst_t> svr_insts_list;
    assert(0 == ds_inst->get_insts_by_name("comp_svr", svr_insts_list));

    /* choose a svr inst randomly */
    svr_inst_t &svr_inst = svr_insts_list[rand() % svr_insts_list.size()];
    RPC_INFO("choose svr instant, name=%s, version=%s, id=%s, ip=%s, port=%u",
            svr_inst.name.c_str(), svr_inst.version.c_str(), svr_inst.id.c_str(), 
            svr_inst.ip.c_str(), svr_inst.port);

    /* marshalling */
    basic_proto inpro;
    inpro.add_matrix(A, A_row, A_col);
    inpro.add_matrix(B, B_row, B_col);
    inpro.add_matrix(C, C_row, C_col);

    std::string req_head = gen_http_head(
            "/multiply", svr_inst.ip, inpro.get_buf_len());
    std::string req_body(inpro.get_buf(), inpro.get_buf_len());
    std::string rsp_head, rsp_body;

    /* TODO: retalk */
    http_talk(svr_inst.ip, svr_inst.port, 
            req_head, req_body, rsp_head, rsp_body);
    
    /* unmarshalling */
    basic_proto outpro(rsp_body.data(), rsp_body.size());
    int *A_bak, *B_bak, *C_bak;
    outpro.read_matrix(A_bak, A_row, A_col);
    outpro.read_matrix(B_bak, B_row, B_col);
    outpro.read_matrix(C_bak, C_row, C_col);

    memcpy(A, A_bak, sizeof(int) * A_row * A_col);
    memcpy(B, B_bak, sizeof(int) * B_row * B_col);
    memcpy(C, C_bak, sizeof(int) * C_row * C_col);
    return;
}
