#ifndef __RPC_NET__
#define __RPC_NET__

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int set_nonblock(int fd);

int send_ex(int fd, char *buf, int expect_len, 
        int flags, int timeout_ms);

int recv_ex(int fd, char *buf, int expect_len, 
        int flags, int timeout_ms);

#endif
