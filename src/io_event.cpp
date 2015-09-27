#include "io_event.h"
#include <stdlib.h>
#include "rpc_net.h"
#include "rpc_log.h"
#include "svr_base.h"

/**************************************
 * io_event
 **************************************/
io_event::io_event(svr_base *svr): 
    m_svr(svr), 
    m_fd(-1), 
    m_io_type('i') { 
}

io_event::~io_event() { 
    if (m_fd != -1) {
        close(m_fd);
        RPC_DEBUG("close fd, fd=%d", m_fd);
    }
    m_fd = -1;
}

void io_event::on_event() { 
    RPC_DEBUG("on_event");
}

/**************************************
 * accept_event
 **************************************/
accept_event::accept_event(svr_base *svr):
    io_event(svr){
}

accept_event::~accept_event() {
}

int accept_event::bind(unsigned short port, int backlog) {

    /* create socket */
    m_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == m_fd) {
        RPC_WARNING("socket() error, errno=%d", errno);
        return -1;
    }
    RPC_DEBUG("svr socket created, fd=%d", m_fd);
    
    /* set to nonblock socket */
    if (-1 == set_nonblock(m_fd)) {
        RPC_WARNING("set_nonblock() error, errno=%d", errno);
        return -1;
    }
    
    /* set reuseaddr */
    int val = 1;
    int ret = setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (-1 == ret) {
        RPC_WARNING("setsockopt() error, errno=%d", errno);
        return -1;
    }
    
    /* bind to port */
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    
    ret = ::bind(m_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (-1 == ret) {
        RPC_WARNING("bind() error, errno=%d", errno);
        return -1;
    }
    RPC_DEBUG("svr socket binded, port=%u", port);
    
    /* listen */
    ret = listen(m_fd, backlog);
    if (-1 == ret) {
        RPC_WARNING("listen() error, errno=%d", errno);
        return -1;
    }
    RPC_DEBUG("svr socket is listening, backlog=%d", backlog);
    
    RPC_INFO("svr binded succ, fd=%d, port=%u, backlog=%d", m_fd, port, backlog);

    this->set_io_type('i');
    return 0;
}

void accept_event::on_event() {

    /* accept new connection */
    struct sockaddr_in remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);
    memset(&remote_addr, 0, sizeof(remote_addr));

    int fd = accept(m_fd, (struct sockaddr*)&remote_addr, &remote_addr_len);
    if (-1 == fd) {
        RPC_WARNING("accept() error, errno=%d", errno);
        return;
    }

    /* set fd to nonblock */
    if (-1 == set_nonblock(fd)) {
        RPC_WARNING("set_nonblock() error, errno=%d", errno);
        close(fd);
        return;
    }

    /* extract ip and port from remote_addr */
    std::string ip(inet_ntoa(remote_addr.sin_addr));
    unsigned short port = ntohs(remote_addr.sin_port);

    RPC_DEBUG("accept new connection, fd=%d, ip=%s, port=%u",
            fd, ip.c_str(), port);

    /* create new io event, bind fd with it */
    http_event *evt = new http_event(m_svr);
    evt->set_ip(ip);
    evt->set_port(port);
    evt->set_fd(fd);
    evt->set_io_type('i');
    evt->set_state("read_head");

    m_svr->add_io_event(this);
    m_svr->add_io_event(evt);
}

/**************************************
 * accept_event
 **************************************/
http_event::http_event(svr_base *svr):
    io_event(svr),
    m_content_len(0) {
}

http_event::~http_event() {
}

void http_event::on_event() {
    if (this->get_state() == "read_head") {
        on_read_head();
    } else if (this->get_state() == "read_body") {
        on_read_body();
    } else {
        on_write();
    }
}

void http_event::on_read_head() {
    char buf[128] = { 0 };

    int len = recv(m_fd, buf, sizeof(buf), 0);
    if (len == 0) { 
        RPC_WARNING("client closed socket, fd=%d, ip=%s, port=%u", 
                m_fd, m_ip.c_str(), m_port);
        delete this;
        return;
    } 
    if (len < 0) { 
        RPC_WARNING("recv() error, fd=%d, errno=%d, ip=%s, port=%u", 
                m_fd, errno, m_ip.c_str(), m_port);
        delete this;
        return;
    } 

    RPC_DEBUG("recv head, fd=%d, read_len=%d", m_fd, len);
    m_head.append(buf, len);
    std::size_t pos = m_head.find("\r\n\r\n");

    if (pos == std::string::npos) {
        /* read head not done */
        this->set_state("read_head");
        m_svr->add_io_event(this);
    } else {
        /* read head done */
        RPC_DEBUG("read head done, fd=%d", m_fd);

        m_body = m_head.substr(pos + strlen("\r\n\r\n"));
        m_head = m_head.substr(0, pos);

        /* parse content length */
        m_content_len = 0;
        char *pos_contlen = strcasestr((char*)m_head.c_str(), "content-length");
        if (NULL != pos_contlen) {
            char *pos_colon = strcasestr(pos_contlen, ":");
            if (NULL != pos_colon) {
                m_content_len = atoi(pos_colon + 1);
            }
        }
        RPC_DEBUG("extracted content_len, fd=%d, content_len=%d",
                m_fd, m_content_len);

        if (m_body.size() < m_content_len) {
            this->set_state("read_body");
            m_svr->add_io_event(this);
        } else {
            m_body.resize(m_content_len);
            this->set_state("compute");
            this->callback();
        }
    }
}

void http_event::on_compute() {
    RPC_DEBUG("compute %s", m_head.c_str());
    m_rsp = m_head;
    this->set_state("write");
    this->set_io_type('o');
    m_svr->add_io_event(this);
}

void http_event::on_read_body() {
    std::string buf;
    buf.resize(m_content_len - m_body.size());

    int len = recv(m_fd, (char*)buf.data(), buf.size(), 0);
    if (len == 0) { 
        RPC_WARNING("client closed socket, fd=%d, ip=%s, port=%u", 
                m_fd, m_ip.c_str(), m_port);
        delete this;
        return;
    } 
    if (len < 0) { 
        RPC_WARNING("recv() error, fd=%d, errno=%d, ip=%s, port=%u", 
                m_fd, errno, m_ip.c_str(), m_port);
        delete this;
        return;
    }
    RPC_DEBUG("recv body, fd=%d, read_len=%d", m_fd, len);
    m_body.append(buf);
    if (m_body.size() < m_content_len) {
        this->set_state("read_body");
        m_svr->add_io_event(this);
    } else {
        m_body.resize(m_content_len);
        this->set_state("compute");
        this->callback();
    }
}

void http_event::on_write() {
    int len = send(m_fd, (char*)m_rsp.data(), m_rsp.size(), 0);
    if (len <= 0) { 
        RPC_WARNING("send() error, fd=%d, errno=%d, ip=%s, port=%u", 
                m_fd, errno, m_ip.c_str(), m_port);
        delete this;
        return;
    }
    RPC_DEBUG("send, fd=%d, send_len=%d", m_fd, len);
    m_rsp = m_rsp.substr(len);
    if (m_rsp.size() > 0) {
        this->set_state("write");
        m_svr->add_io_event(this);
    } else {
        RPC_DEBUG("session over, fd=%d", m_fd);
        delete this;
    }
}

void http_event::callback() {
    m_svr->on_dispatch_task(this);
}
