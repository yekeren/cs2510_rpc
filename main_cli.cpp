#include "rpc_log.h"
#include "add_stub_cli.h"
#include "wc_stub_cli.h"
#include "multiply_stub_cli.h"

int main(int argc, char *argv[]) {
    printf("sum=%d\n", add(10, 4));
    printf("wordcount=%d\n", wc("hello, world"));

    int A[3][1] = { { 1 }, { 2 }, { 3 } };
    int B[1][3] = { { 1, 2, 3} };
    int C[3][3];
    multiply(&A[0][0], 3, 1, &B[0][0], 1, 3, &C[0][0], 3, 3);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%d\t", C[i][j]);
        }
        printf("\n");
    }
    return 0;
}
