#include "cs.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "rpc_log.h"
#include "rpc_http.h"
#include "basic_proto.h"
#include "template.h"
#include "rpc_common.h"

void multiply(int *A, int A_row, int A_col, int *B, int B_row, int B_col, int *C, int C_row, int C_col) { 

svr_inst_t svr_inst;
int ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);
RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u", svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst.port);
basic_proto inpro;
inpro.add_matrix(A, A_row,A_col);
inpro.add_matrix(B, B_row,B_col);
inpro.add_matrix(C, C_row,C_col);
std::string req_head = gen_http_head("/multiply", svr_inst.ip, inpro.get_buf_len());
std::string req_body(inpro.get_buf(), inpro.get_buf_len());
std::string rsp_head, rsp_body;
http_talk(svr_inst.ip, svr_inst.port, req_head, req_body, rsp_head, rsp_body);
basic_proto outpro(rsp_body.data(), rsp_body.size());
int *A_bak;
outpro.read_matrix(A_bak, A_row, A_col);
memcpy(A, A_bak, sizeof(int) * A_row * A_col);
int *B_bak;
outpro.read_matrix(B_bak, B_row, B_col);
memcpy(B, B_bak, sizeof(int) * B_row * B_col);
int *C_bak;
outpro.read_matrix(C_bak, C_row, C_col);
memcpy(C, C_bak, sizeof(int) * C_row * C_col);
}

int max(int data_len, int *data) { 

svr_inst_t svr_inst;
int ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);
RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u", svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst.port);
basic_proto inpro;
inpro.add_array(data, data_len);
std::string req_head = gen_http_head("/max", svr_inst.ip, inpro.get_buf_len());
std::string req_body(inpro.get_buf(), inpro.get_buf_len());
std::string rsp_head, rsp_body;
http_talk(svr_inst.ip, svr_inst.port, req_head, req_body, rsp_head, rsp_body);
basic_proto outpro(rsp_body.data(), rsp_body.size());
outpro.read_array(data, data_len);
int retval;
outpro.read_int(retval);
return retval;
}

int add(int a, int b) { 

svr_inst_t svr_inst;
int ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);
RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u", svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst.port);
basic_proto inpro;
inpro.add_int(a);
inpro.add_int(b);
std::string req_head = gen_http_head("/add", svr_inst.ip, inpro.get_buf_len());
std::string req_body(inpro.get_buf(), inpro.get_buf_len());
std::string rsp_head, rsp_body;
http_talk(svr_inst.ip, svr_inst.port, req_head, req_body, rsp_head, rsp_body);
basic_proto outpro(rsp_body.data(), rsp_body.size());
outpro.read_int(a);
outpro.read_int(b);
int retval;
outpro.read_int(retval);
return retval;
}

int wc(char *str, int str_len) { 

svr_inst_t svr_inst;
int ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);
RPC_INFO("server verified, id=%d, version=%s, ip=%s, port=%u", svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst.port);
basic_proto inpro;
std::string req_head = gen_http_head("/wc", svr_inst.ip, inpro.get_buf_len());
std::string req_body(inpro.get_buf(), inpro.get_buf_len());
std::string rsp_head, rsp_body;
http_talk(svr_inst.ip, svr_inst.port, req_head, req_body, rsp_head, rsp_body);
basic_proto outpro(rsp_body.data(), rsp_body.size());
int retval;
outpro.read_int(retval);
return retval;
}

