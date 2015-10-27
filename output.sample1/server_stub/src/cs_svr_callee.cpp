#include "cs.h"
#include <string>
#include "rpc_log.h"

void print_matrix(int *A, int A_row, int A_col) {
    for (int i = 0; i < A_row; ++i) {
        for (int j = 0; j < A_col; ++j) {
            RPC_TERMINAL("%4d", A[i * A_col + j]);
        }
        RPC_TERMINAL("\n");
    }
}

void multiply(int *A, int A_row, int A_col, int *B, int B_row, int B_col, int *C, int C_row, int C_col){
    RPC_TERMINAL("matrix A:\n");
    print_matrix(A, A_row, A_col);
    RPC_TERMINAL("matrix B:\n");
    print_matrix(B, B_row, C_col);

    for (int i = 0; i < A_row; ++i) {
        for (int j = 0; j < B_col; ++j) {
            C[i * B_col + j] = 0;
            for (int k = 0; k < A_col; ++k) {
                C[i * B_col + j] += A[i * A_col + k] * B[k * B_col + j];
            }
        }
    }
    RPC_TERMINAL("matrix C:\n");
    print_matrix(C, C_row, C_col);
}

int max(int data_len, int *data){
    int max_v = 0;
    for (int i = 0; i < data_len; ++i) {
        if (data[i] > max_v) {
            max_v = data[i];
        }
    }
    RPC_TERMINAL("max value is %d\n", max_v);
    for (int i = 0; i < data_len; ++i) {
        RPC_TERMINAL("%4d", data[i]);
    }
    RPC_TERMINAL("\n");
    return max_v;
}

int add(int a, int b){
    RPC_TERMINAL("%d + %d = %d\n", a, b, a + b);
    return a + b;
}

int wc(char *str, int str_len){
    int count = 0;
    for (int i = 0; i < str_len; ++i) {
        if (str[i] == ' ') {
            ++count;
        }
    }
    RPC_TERMINAL("there are %d words in:\n %s\n", 
            1 + count, std::string(str, str_len).c_str());
    return 1 + count;
}

