#include "add_stub.h"
#include "ds_svc.h"
#include "add_proto.h"

int add_stub(int a, int b) {
    svr_t svr_inst;
    ds_svc *ds_inst = ds_svc::instance();
    ds_inst->get_svr_by_id("add", "1.0.0", svr_inst);

    add_marshal am;
    am.set_a(a);
    am.set_b(b);

    const *char *req = am.get_send_buf();
    int req_len = am.get_send_buf_len();

    cs_svc *cs_inst = cs_svc::instance();
    cs_inst->get_response(svr_inst.ip, svr_inst.port, 
            req, req_len, &rsp, sizeof(rsp));

    return rsp.ret;
}
