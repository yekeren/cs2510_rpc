#include "add_stub_cli.h"
#include <assert.h>
#include <stdlib.h>
#include "ds_svc.h"
#include "rpc_log.h"
#include "add_proto.h"
#include "rpc_http.h"


int add(int a, int b) {
    ds_svc *ds_inst = ds_svc::instance();

    /* get svr insts from directory service */
    std::vector<svr_inst_t> svr_insts_list;
    int ret = ds_inst->get_insts_by_name("kexin", svr_insts_list);
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
    add_proto ap;
    ap.set_a(a);
    ap.set_b(b);

    const char *buf = ap.encode();
    int buf_len = ap.get_buf_len();


    std::string req_head;
    req_head += std::string("GET /server-config.xml?name=") + svr_inst.name.c_str() + " HTTP/1.1\r\n";
    req_head += "Host: " DIR_SVR_HOST "\r\n";
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
    add_proto ap_rsp;
    
    ap_rsp.decode(rsp_body.data(), rsp_body.size());

    a = ap_rsp.get_a();
    b = ap_rsp.get_b();
    int retval = ap_rsp.get_retval();
    return retval;

    //return retval;
}
