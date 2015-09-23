#include "ds_svc.h"

ds_svc::ds_svc() {
}

ds_svc::~ds_svc() {
}

ds_svc *ds_svc::instance() {
    return NULL;
}

void ds_svc::destroy() {
}

int ds_svc::get_svr_by_id(const std::string &id, 
        const std::string &version,
        svr_t &svr_inst) {

    return 0;
}
