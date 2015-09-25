#include "ds_svc.h"

ds_svc *ds_svc::m_pthis = NULL;

ds_svc::ds_svc() {
}

ds_svc::~ds_svc() {
}

ds_svc *ds_svc::instance() {
    if (NULL == m_pthis) {
        m_pthis = new ds_svc;
    }
    return m_pthis;;
}

void ds_svc::destroy() {
    if (NULL != m_pthis) {
        delete m_pthis;
    }
    m_pthis = NULL;
}

int ds_svc::get_instances_by_name(const std::string &name,
        std::vector<svr_inst_t> &svr_instants_list) {


    return 0;
}
