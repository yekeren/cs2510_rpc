#include "rpc_log.h"
#include "add_stub_cli.h"
#include "wc_stub_cli.h"

int multiply(int m, int n, int k[m][n]) {
    return 0;
}

int main(int argc, char *argv[]) {
    printf("sum=%d\n", add(10, 4));
    printf("len=%d\n", wc("hello, world"));
    return 0;
}
