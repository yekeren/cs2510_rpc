#include "ds_svc.h"
#include "rpc_net.h"
#include "rpc_log.h"
#include "common_def.h"

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

int ds_svc::get_ips_by_host(const std::string &host_name,
        std::vector<std::string> &ips_list) {

    /* getaddrinfo */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res0 = NULL;
    int error = getaddrinfo(DIR_SVR_HOST, "http", &hints, &res0);
    if (error) {
        /* not reached */
        RPC_WARNING("host name is not reach, host=%s, err=%s", 
                host_name.c_str(), gai_strerror(error));
        return -1;
    }

    for (struct addrinfo *res = res0; res; res = res->ai_next) {
        std::string ip(inet_ntoa(((struct sockaddr_in*)res->ai_addr)->sin_addr));
        ips_list.push_back(ip);

        RPC_DEBUG("resolve ip from host, ip=%s", ip.c_str());
    }
    freeaddrinfo(res0);

    if (0 == ips_list.size()) {
        RPC_WARNING("no ip address is found, host=%s", 
                host_name.c_str());
        return -1;
    }
    return 0;
}

int ds_svc::get_instances_by_name(const std::string &name,
        std::vector<svr_inst_t> &svr_instants_list) {

    /* get ips_list by host name */
    std::vector<std::string> ips_list;
    if (0 > get_ips_by_host(DIR_SVR_HOST, ips_list)) {
        RPC_WARNING("get_ip_by_host() error, host=%s", DIR_SVR_HOST);
        return -1;
    }

    /* connect to remote server */
    int fd = -1;
    for (int i = 0; i < (int)ips_list.size(); ++i) {
        fd = connect_ex((char*)ips_list[i].c_str(), 
                DIR_SVR_PORT, DIR_SVR_CONN_TIMEOUT);
        if (-1 == fd) {
            RPC_WARNING("connect_ex() failed, try=%d, fd=%d, ip=%s, port=%u", 
                    i, fd, ips_list[i].c_str(), DIR_SVR_PORT);
        } else {
            break;
        }
    }
    if (fd < 0) {
        RPC_WARNING("get_instances_by_name() failed, name=%s", name.c_str());
        return -1;
    }

    /* send request to remote server */
    std::string request_head;
    request_head += "GET /server-config.xml HTTP/1.1\r\n";
    request_head += "Host: "DIR_SVR_HOST"\r\n";
    request_head += "\r\n\r\n";

    int send_len = send_ex(fd, (char*)request_head.c_str(), 
            request_head.length(), 0, DIR_SVR_SEND_TIMEOUT);
    if (send_len < 0) {
        RPC_WARNING("send_ex() error, fd=%d", fd);
        return -1;
    }

    /* recv response from remote server */
    char buf[60] = { 0 };
    int recv_len = recv_ex(fd, buf, sizeof(buf), 
            0, DIR_SVR_RECV_TIMEOUT);
    if (recv_len < 0) {
        RPC_WARNING("recv_ex() error, fd=%d", fd);
        return -1;
    }
    RPC_DEBUG("%s", buf);


    return 0;
}
