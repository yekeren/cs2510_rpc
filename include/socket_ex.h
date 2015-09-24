
int connect_ex() {
    select()/poll()/epoll()
    connect()
}

int send_ex(uint32_t timeout_ms) {
    select()/poll()/epoll()
    send()

    return -1;
}

int recv_ex(socket fd, void *buf, int buflen, int timeout_ms) {
    timeval tv;
    while (true) {
        int ret = poll()
        if (ret > 0) {
            r = recv(fd, buf, buflen)
            if r == 0:
                return -1
            else:
        } else if ret == 0{
        }
    }
}
