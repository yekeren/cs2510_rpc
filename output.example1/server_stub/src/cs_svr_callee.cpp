#include "cs.h"

void multiply(int *A, int A_row, int A_col, int *B, int B_row, int B_col, int *C, int C_row, int C_col){
    for (int i = 0; i < A_row; ++i) {
        for (int j = 0; j < B_col; ++j) {
            C[i * B_col + j] = 0;
            for (int k = 0; k < A_col; ++k) {
                C[i * B_col + j] += A[i * A_col + k] * B[k * B_col + j];
            }
        }
    }
}

int max(int data_len, int *data){
    int m = 0;
    for (int i = 0; i < data_len; ++i) {
        if (data[i] > m) {
            m = data[i];
        }
    }
    return m;
}

int add(int a, int b){
    return a + b;
}

int wc(char *str, int str_len){
    int count = 0;
    for (int i = 0; i < str_len; ++i) {
        if (str[i] == ' ') {
            ++count;
        }
    }
    return count + 1;
}

