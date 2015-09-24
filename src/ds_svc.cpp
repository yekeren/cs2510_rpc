#include "cs_svc.h"

cs_svc::cs_svc() {
}

cs_svc::~cs_svc() {
}

cs_svc *cs_svc::instance() {
    return NULL;
}

void cs_svc::destroy() {
}

int cs_svc::get_response(const std::string &ip, 
        uint16_t port,
        void *req_buf, int req_buf_len,
        void *rsp_buf, int &rsp_buf_len) {

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    connet();
    send()/write()
    /* http */
    ""
    receive()/read()
    close()

    //socket;
    struct head {
        int body_len;
    };
    send()
    body_len = receive(4);
    reaceive(body_len);
}
