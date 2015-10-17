#include "cs_svr.h"
#include <string.h>
#include <assert.h>
#include "common_def.h"
#include "rpc_net.h"
#include "rpc_log.h"
#include "template.h"
#include "ezxml.h"
#include "basic_proto.h"

#define RPC_ID 1
#define RPC_NAME "computation_service"
#define RPC_VERSION "1.0.0"

void print_matrix(int *k, int row, int col) {
    RPC_TERMINAL("%d * %d\n", row, col);
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            RPC_TERMINAL("%4d", k[i * col + j]);
        }
        RPC_TERMINAL("\n");
    }
    RPC_TERMINAL("\n");
}

int add(int a, int b) {
    RPC_TERMINAL("%d + %d = %d\n", a, b, a + b);
    return a + b;
}

int wc(const std::string &str) {
    RPC_TERMINAL("wc: %s\n", str.c_str());
    return str.length();
}

int max(int size, int *data) {
    int retval = 0;
    for (int i = 0; i < size; ++i) {
        if (data[i] > retval) {
            retval = data[i];
        }
        RPC_TERMINAL("%4d", data[i]);
    }
    RPC_TERMINAL("max=%d", retval);
    return retval;
}

void multiply(int *A, int A_row, int A_col,
        int *B, int B_row, int B_col,
        int *C, int C_row, int C_col) {

    RPC_TERMINAL("matrix A:\n");
    print_matrix(A, A_row, A_col);

    RPC_TERMINAL("matrix B:\n");
    print_matrix(B, B_row, B_col);

    memset(C, 0, sizeof(int) * C_row * C_col);
    for (int i = 0; i < A_row; ++i) {
        for (int j = 0; j < B_col; ++j) {
            for (int k = 0; k < A_col; ++k) {
                C[i * C_col + j] += A[i * A_col + k] * B[k * B_col + j];
            }
        }
    }
    RPC_TERMINAL("matrix C:\n");
    print_matrix(C, C_row, C_col);

}

cs_event::cs_event(svr_base *svr): 
    http_event(svr) {
}

void cs_event::on_process() {

    std::string req_head = get_head();
    std::string req_body = get_body();

    RPC_DEBUG("ip=%s, port=%u, method=%s, uri=%s, version=%s", 
            get_ip().c_str(), get_port(), 
            get_method().c_str(), get_uri().c_str(), get_version().c_str());

    std::string rsp_head, rsp_body;
    this->dsptch_http_request(get_uri(), req_body, rsp_head, rsp_body);

    /* set response */
    this->set_response(rsp_head + rsp_body);

    this->set_state("write");
    this->set_io_type('o');
    m_svr->add_io_event(this);
}

void cs_event::do_default(const std::string &uri,
        const std::string &req_body, std::string &rsp_head, std::string &rsp_body) {

    RPC_WARNING("invalid request from client, uri=%s, ip=%s, port=%u", 
            uri.c_str(), get_ip().c_str(), get_port());

    ezxml_t root = ezxml_new("message");
    ezxml_set_txt(root, "invalid request");
    rsp_body = ezxml_toxml(root);
    ezxml_free(root);

    rsp_head = gen_http_head("404 Not Found", rsp_body.size());
}

void cs_event::dsptch_http_request(const std::string &uri, 
        const std::string &req_body, std::string &rsp_head, std::string &rsp_body) {

    if (uri.find("/get_svr_id") == 0) {
        do_get_svr_id(req_body, rsp_head, rsp_body);
    } 
    else if (uri.find("/add") == 0) {
        process_add(req_body, rsp_head, rsp_body);
    }
    else if (uri.find("/max")==0){
        process_max(req_body, rsp_head, rsp_body);
    }
    else if (uri.find("/wc")==0){
        process_wc(req_body, rsp_head, rsp_body);
    }
    else if (uri.find("/multiply")==0){
        process_multiply(req_body, rsp_head, rsp_body);
    }
    else {
        do_default(uri, req_body, rsp_head, rsp_body);
    }
}

void cs_event::do_get_svr_id(const std::string &req_body, 
        std::string &rsp_head, std::string &rsp_body) {

    char buf[1024] = { 0 };

    ezxml_t root = ezxml_new("server");

    sprintf(buf, "%d", RPC_ID);
    ezxml_set_txt(ezxml_add_child(root, "id", 0), buf);

    ezxml_set_txt(ezxml_add_child(root, "name", 0), RPC_NAME);
    ezxml_set_txt(ezxml_add_child(root, "version", 0), RPC_VERSION);

    rsp_body = ezxml_toxml(root);
    ezxml_free(root);

    rsp_head = gen_http_head("200 OK", rsp_body.size());
    rsp_body.assign(rsp_body.data(), rsp_body.size());
}

void cs_event::process_add(const std::string &req_body, 
        std::string &rsp_head, std::string &rsp_body) {

    basic_proto bpin(req_body.data(), req_body.size());
    basic_proto bpout;

    int a, b, retval;
    bpin.read_int(a);
    bpin.read_int(b);

    retval = add(a, b);

    bpout.add_int(a);
    bpout.add_int(b);
    bpout.add_int(retval);

    rsp_head = gen_http_head("200 OK", bpout.get_buf_len());
    rsp_body.assign(bpout.get_buf(), bpout.get_buf_len());
}
    
void cs_event::process_max(const std::string &req_body,
        std::string &rsp_head, std::string &rsp_body) {

    basic_proto bpin(req_body.data(), req_body.size());
    basic_proto bpout;

    int size, *data, retval;
    bpin.read_array(data, size);

    retval = max(size, data);

    bpout.add_array(data, size);
    bpout.add_int(retval);

    rsp_head = gen_http_head("200 OK", bpout.get_buf_len());
    rsp_body.assign(bpout.get_buf(), bpout.get_buf_len());
}

void cs_event::process_wc(const std::string &req_body,
        std::string &rsp_head, std::string & rsp_body){

    basic_proto bpin(req_body.data(), req_body.size());
    basic_proto bpout;

    int str_len;
    char *str;
    bpin.read_string(str_len, str);

    int retval = wc(std::string(str, str_len));

    bpout.add_string(str_len, str);
    bpout.add_int(retval);

    rsp_head = gen_http_head("200 OK", bpout.get_buf_len());
    rsp_body.assign(bpout.get_buf(), bpout.get_buf_len());
}

void cs_event::process_multiply(const std::string &req_body, 
        std::string &rsp_head, std::string &rsp_body) {

    basic_proto bpin(req_body.data(), req_body.size());
    basic_proto bpout;

    int *A = NULL, *B = NULL, *C = NULL;
    int A_row, A_col;
    int B_row, B_col;
    int C_row, C_col;

    bpin.read_matrix(A, A_row, A_col);
    bpin.read_matrix(B, B_row, B_col);
    bpin.read_matrix(C, C_row, C_col);

    multiply(A, A_row, A_col, B, B_row, B_col, C, C_row, C_col);

    bpout.add_matrix(A, A_row, A_col);
    bpout.add_matrix(B, B_row, B_col);
    bpout.add_matrix(C, C_row, C_col);

    rsp_head = gen_http_head("200 OK", bpout.get_buf_len());
    rsp_body.assign(bpout.get_buf(), bpout.get_buf_len());
}

cs_svr::cs_svr() {
}

cs_svr::~cs_svr() {
}

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
