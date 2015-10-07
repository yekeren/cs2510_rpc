#include "wc_stub_cli.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "ds_svc.h"
#include "rpc_log.h"
#include "wc_proto.h"
#include "rpc_http.h"


int wc(std::string str) {
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

    /* data marshalling */
    //return 0;
    basic_proto ap;
    int retval;
    ap.add_string(str.size(), str.data(), retval);

    const char *buf = ap.get_buf();
    int buf_len = ap.get_buf_len();


    std::string req_head;
    char tbuf[32] = { 0 };
    req_head += "POST /wc HTTP/1.1\r\n";
    req_head += "Host: " + svr_inst.ip + "\r\n";
    req_head += "Content-Length: ";
    sprintf(tbuf, "%d", buf_len);
    req_head += tbuf;
    req_head += "\r\n\r\n";

    std::string req_body(buf, buf_len);

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
    int str_len;
    char * back_str;
    ap_rsp.read_string(str_len, back_str, retval);
    return retval;

    //return retval;
}
