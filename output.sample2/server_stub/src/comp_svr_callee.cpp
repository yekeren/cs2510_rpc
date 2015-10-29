#include "comp.h"
#include "rpc_log.h"

void print_matrix(int *A, int A_row, int A_col) {
    for (int i = 0; i < A_row; ++i) {
        for (int j = 0; j < A_col; ++j) {
            RPC_TERMINAL("%4d", A[i * A_col + j]);
        }
        RPC_TERMINAL("\n");
    }
}

void print_array(int *A, int A_len) {
    for (int i = 0; i < A_len; ++i) {
        RPC_TERMINAL("%4d", A[i]);
    }
    RPC_TERMINAL("\n");
}

double test1_divide(double a, double b){
    RPC_TERMINAL("%.4lf / %.4lf = %.4lf\n", a, b, a / b);
    return a / b;
}

void test1_transpose(int *A, int A_row, int A_col, int *B, int B_row, int B_col){
    RPC_TERMINAL("original matrix:\n");
    print_matrix(A, A_row, A_col);

    for (int i = 0; i < A_row; ++i) {
        for (int j = 0; j < A_col; ++j) {
            B[j * B_col + i] = A[i * A_col + j];
        }
    }

    RPC_TERMINAL("transpose result:\n");
    print_matrix(B, B_row, B_col);
}

void test1_sort(int data_len, int *data){
    RPC_TERMINAL("original array:\n");
    print_array(data, data_len);

    for (int i = 0; i < data_len; ++i) {
        for (int j = i + 1; j < data_len; ++j) {
            if (data[i] > data[j]) {
                int tmp = data[j];
                data[j] = data[i];
                data[i] = tmp;
            }
        }
    }

    RPC_TERMINAL("sort result:\n");
    print_array(data, data_len);
}

int test1_wordcount(char *str, int str_len){
    return 0;
}

