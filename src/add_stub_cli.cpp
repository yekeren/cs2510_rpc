#include "add_stub_cli.h"
#include "ds_svc.h"
#include "rpc_log.h"

int add(int a, int b) {
    ds_svc *ds_inst = ds_svc::instance();

    std::vector<svr_inst_t> svr_instants_list;
    ds_inst->get_instances_by_name("kexin", svr_instants_list);

    for (int i = 0; i < (int)svr_instants_list.size(); ++i) {
        svr_inst_t &svr_inst = svr_instants_list[i];
        RPC_DEBUG("svr instant [%d], name=%s, version=%s, id=%d, ip=%s, port=%u",
                i, svr_inst.name.c_str(), svr_inst.version.c_str(), svr_inst.id, 
                svr_inst.ip.c_str(), svr_inst.port);
    }
    return a + b;
}
