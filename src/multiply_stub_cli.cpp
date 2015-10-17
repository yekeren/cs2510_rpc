#include "multiply_stub_cli.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "rpc_log.h"
#include "rpc_http.h"
#include "basic_proto.h"
#include "template.h"
#include "rpc_common.h"

#define DS_IP "127.0.0.1"
#define DS_PORT 8000
#define RPC_ID 1
#define RPC_NAME "computation_service"
#define RPC_VERSION "1.0.0"

void multiply(int *A, int A_row, int A_col,
        int *B, int B_row, int B_col,
        int *C, int C_row, int C_col) {

    svr_inst_t svr_inst;
    int ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);

    RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u",
            svr_inst.id, svr_inst.version.c_str(),
            svr_inst.ip.c_str(), svr_inst.port);

    exit(0);

    ///* marshalling */
    //basic_proto inpro;
    //inpro.add_matrix(A, A_row, A_col);
    //inpro.add_matrix(B, B_row, B_col);
    //inpro.add_matrix(C, C_row, C_col);

    //std::string req_head = gen_http_head(
    //        "/multiply", svr_inst.ip, inpro.get_buf_len());
    //std::string req_body(inpro.get_buf(), inpro.get_buf_len());
    //std::string rsp_head, rsp_body;

    ///* TODO: retalk */
    //http_talk(svr_inst.ip, svr_inst.port, 
    //        req_head, req_body, rsp_head, rsp_body);
    //
    ///* unmarshalling */
    //basic_proto outpro(rsp_body.data(), rsp_body.size());
    //int *A_bak, *B_bak, *C_bak;
    //outpro.read_matrix(A_bak, A_row, A_col);
    //outpro.read_matrix(B_bak, B_row, B_col);
    //outpro.read_matrix(C_bak, C_row, C_col);

    //memcpy(A, A_bak, sizeof(int) * A_row * A_col);
    //memcpy(B, B_bak, sizeof(int) * B_row * B_col);
    //memcpy(C, C_bak, sizeof(int) * C_row * C_col);
    return;
}
