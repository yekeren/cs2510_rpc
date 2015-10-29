#include "comp.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rpc_log.h"
#include "rpc_http.h"
#include "basic_proto.h"
#include "template.h"
#include "rpc_common.h"

double test1_divide(double a, double b) {

    svr_inst_t svr_inst;
    int ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);
    if (ret_val == -1) {
        RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u", 
                svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst.port);
        return 0;
    }
    basic_proto inpro;
    inpro.add_double(a);
    inpro.add_double(b);

    std::string rsp_head, rsp_body;
    rpc_call_by_id(id_test1_divide, svr_inst.ip, svr_inst.port, inpro, rsp_head, rsp_body);
    basic_proto outpro(rsp_body.data(), rsp_body.size());

    outpro.read_double(a);
    outpro.read_double(b);

    double retval;
    outpro.read_double(retval);
    return retval;
}

void test1_transpose(int *A, int A_row, int A_col, int *B, int B_row, int B_col) {

    svr_inst_t svr_inst;
    int ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);
    if (ret_val == -1) {
        RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u", 
                svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst.port);
        return;
    }
    basic_proto inpro;
    inpro.add_matrix(A, A_row, A_col);
    inpro.add_matrix(B, B_row, B_col);

    std::string rsp_head, rsp_body;
    rpc_call_by_id(id_test1_transpose, svr_inst.ip, svr_inst.port, inpro, rsp_head, rsp_body);
    basic_proto outpro(rsp_body.data(), rsp_body.size());

    int *A_bak;
    outpro.read_matrix(A_bak, A_row, A_col);
    memcpy(A, A_bak, sizeof(int) * A_row * A_col);
    int *B_bak;
    outpro.read_matrix(B_bak, B_row, B_col);
    memcpy(B, B_bak, sizeof(int) * B_row * B_col);

}

void test1_sort(int data_len, int *data) {

    svr_inst_t svr_inst;
    int ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);
    if (ret_val == -1) {
        RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u", 
                svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst.port);
        return;
    }
    basic_proto inpro;
    inpro.add_array(data, data_len);

    std::string rsp_head, rsp_body;
    rpc_call_by_id(id_test1_sort, svr_inst.ip, svr_inst.port, inpro, rsp_head, rsp_body);
    basic_proto outpro(rsp_body.data(), rsp_body.size());

    int *data_bak;
    outpro.read_array(data_bak, data_len);
    memcpy(data, data_bak, sizeof(int) * data_len);

}

int test1_wordcount(char *str, int str_len) {

    svr_inst_t svr_inst;
    int ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);
    if (ret_val == -1) {
        RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u", 
                svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst.port);
        return 0;
    }
    basic_proto inpro;
    inpro.add_string(str_len, str);

    std::string rsp_head, rsp_body;
    rpc_call_by_id(id_test1_wordcount, svr_inst.ip, svr_inst.port, inpro, rsp_head, rsp_body);
    basic_proto outpro(rsp_body.data(), rsp_body.size());

    char *str_bak;
    outpro.read_string(str_len, str_bak);

    int retval;
    outpro.read_int(retval);
    return retval;
}

