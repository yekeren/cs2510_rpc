#ifndef __comp_h__
#define __comp_h__

#define RPC_ID 11111
#define RPC_NAME "comp"
#define RPC_VERSION "1.0.0"
#define DS_IP "127.0.0.1"
#define DS_PORT 8000
#define id_test1_divide 0
#define id_test1_transpose 1
#define id_test1_sort 2
#define id_test1_wordcount 3

double test1_divide(double a, double b);

void test1_transpose(int *A, int A_row, int A_col, int *B, int B_row, int B_col);

void test1_sort(int data_len, int *data);

int test1_wordcount(char *str, int str_len);

#endif
