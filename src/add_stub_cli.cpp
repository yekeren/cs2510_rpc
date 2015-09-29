#include "add_stub_cli.h"
#include <assert.h>
#include <stdlib.h>
#include "ds_svc.h"
#include "rpc_log.h"
#include "add_proto.h"

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
    return 0;
    add_proto ap;
    ap.set_a(a);
    ap.set_b(b);
    ap.set0_ret(0);

    /* request for computing */

    /* data unmarshalling */
    a = ap.get_a();
    b = ap.get_b();
    int retval = ap.get0_ret();

    return retval;
}
