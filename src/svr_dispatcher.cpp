struct header {
    int bodylen;
    char proc_name[16];
}

int add(int a, int b) {
    return a + b;
}

void *compute_thread(void *param) {
    //file descriptor
    int fd = (int)param;
    body_len = read_ex(fd, 4)
    proc_name = read_ex(fd, 16)

    buf = read_ex(fd, body_len)

    if (proc_name == "add") {
        add_mashal am;
        am.decode(buf, body_len)
        a = am.get_a();
        b = am.get_b();
        ret = add(a, b)
    }
    write_ex(fd);
    close(fd);
}

void *long_connectcompute_thread(void *param) {
    //file descriptor
    int fd = (int)param;

    while (running) {
        int ret = receive_ex(fd, 30s)

        if (ret == 0) {
            break;
        }
        write_ex()
    }
}


int main_loop() {
    s = socket()
    bind(s)
    listen(80)
    while (true) {
        new_fd = accept(s)
        pthread_create(fd, compute_thread);
    }
}
