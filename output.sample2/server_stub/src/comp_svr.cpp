#include "comp.h"
#include "comp_svr.h"
#include <string.h>
#include "template.h"
#include "rpc_net.h"
#include "rpc_log.h"
#include "rpc_common.h"
#include "ezxml.h"
#include "basic_proto.h"

/**************************************
 * comp_event
 **************************************/
comp_event::comp_event(svr_base *svr): 
    http_event(svr) {
}

void comp_event::on_process() {

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

void comp_event::do_default(const std::string &uri,
        const std::string &req_body, std::string &rsp_head, std::string &rsp_body) {

    RPC_WARNING("invalid request from client, uri=%s, ip=%s, port=%u", 
            uri.c_str(), get_ip().c_str(), get_port());

    ezxml_t root = ezxml_new("message");
    ezxml_set_txt(root, "invalid request");
    rsp_body = ezxml_toxml(root);
    ezxml_free(root);

    rsp_head = gen_http_head("404 Not Found", rsp_body.size());
}

void comp_event::dsptch_http_request(const std::string &uri, 
        const std::string &req_body, std::string &rsp_head, std::string &rsp_body) {
    if (uri.find("/get_svr_id") == 0) {
        do_get_svr_id(req_body, rsp_head, rsp_body);
    }
    else if (uri.find("/call_by_id?id=") == 0) {
        int id = atoi(uri.substr(strlen("/call_by_id?id=")).c_str());
        switch (id) {
            case id_test1_divide:
                process_test1_divide(req_body, rsp_head, rsp_body);
                break;
            case id_test1_transpose:
                process_test1_transpose(req_body, rsp_head, rsp_body);
                break;
            case id_test1_sort:
                process_test1_sort(req_body, rsp_head, rsp_body);
                break;
            case id_test1_wordcount:
                process_test1_wordcount(req_body, rsp_head, rsp_body);
                break;
            default:
                RPC_WARNING("invalid request %s", uri.c_str());
        }
    }
    else {
        RPC_WARNING("invalid request %s", uri.c_str());
    }
}

void comp_event::do_get_svr_id(const std::string &req_body, 
        std::string &rsp_head, std::string &rsp_body) {

    ezxml_t root = ezxml_new("server");
    ezxml_set_txt(ezxml_add_child(root, "id", 0), num_to_str(RPC_ID).c_str());
    ezxml_set_txt(ezxml_add_child(root, "name", 0), RPC_NAME);
    ezxml_set_txt(ezxml_add_child(root, "version", 0), RPC_VERSION);

    rsp_body = ezxml_toxml(root);
    ezxml_free(root);

    rsp_head = gen_http_head("200 OK", rsp_body.size());
    rsp_body.assign(rsp_body.data(), rsp_body.size());
}

void comp_event::process_test1_divide(const std::string &req_body,
        std::string &rsp_head, std::string &rsp_body) {

    basic_proto proto_in(req_body.data(), req_body.size());
    basic_proto proto_out;

    double a;
    double b;

    proto_in.read_double(a);
    proto_in.read_double(b);
    double ret_val = test1_divide(a, b);
    proto_out.add_double(a);
    proto_out.add_double(b);

    proto_out.add_double(ret_val);

    rsp_head = gen_http_head("200 OK", proto_out.get_buf_len());
    rsp_body.assign(proto_out.get_buf(), proto_out.get_buf_len());
}

void comp_event::process_test1_transpose(const std::string &req_body,
        std::string &rsp_head, std::string &rsp_body) {

    basic_proto proto_in(req_body.data(), req_body.size());
    basic_proto proto_out;

    int *A; int A_row; int A_col;
    int *B; int B_row; int B_col;

    proto_in.read_matrix(A, A_row, A_col);
    proto_in.read_matrix(B, B_row, B_col);
    test1_transpose(A, A_row, A_col, B, B_row, B_col);
    proto_out.add_matrix(A, A_row, A_col);
    proto_out.add_matrix(B, B_row, B_col);

    rsp_head = gen_http_head("200 OK", proto_out.get_buf_len());
    rsp_body.assign(proto_out.get_buf(), proto_out.get_buf_len());
}

void comp_event::process_test1_sort(const std::string &req_body,
        std::string &rsp_head, std::string &rsp_body) {

    basic_proto proto_in(req_body.data(), req_body.size());
    basic_proto proto_out;

    int data_len; int *data;

    proto_in.read_array(data, data_len);
    test1_sort(data_len, data);
    proto_out.add_array(data, data_len);

    rsp_head = gen_http_head("200 OK", proto_out.get_buf_len());
    rsp_body.assign(proto_out.get_buf(), proto_out.get_buf_len());
}

void comp_event::process_test1_wordcount(const std::string &req_body,
        std::string &rsp_head, std::string &rsp_body) {

    basic_proto proto_in(req_body.data(), req_body.size());
    basic_proto proto_out;

    char *str; int str_len;

    proto_in.read_string(str_len, str);
    int ret_val = test1_wordcount(str, str_len);
    proto_out.add_string(str_len, str);

    proto_out.add_int(ret_val);

    rsp_head = gen_http_head("200 OK", proto_out.get_buf_len());
    rsp_body.assign(proto_out.get_buf(), proto_out.get_buf_len());
}

    
/**************************************
 * comp_svr
 **************************************/
comp_svr::comp_svr() {
}

comp_svr::~comp_svr() {
}

io_event *comp_svr::create_event(int fd,
        const std::string &ip, unsigned short port) {

    comp_event *evt = new comp_event(this);
    evt->set_fd(fd);
    evt->set_ip(ip);
    evt->set_port(port);

    evt->set_io_type('i');
    evt->set_state("read_head");
    evt->set_timeout(RPC_RECV_TIMEOUT);
    return (io_event*)evt;
}
