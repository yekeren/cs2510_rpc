#ifndef __cs_h__
#define __cs_h__

#define RPC_ID 2342134
#define RPC_NAME "cs"
#define RPC_VERSION "1.0.0"
#define DS_IP "127.0.0.1"
#define DS_PORT 8000
#define id_multiply 0
#define id_max 3
#define id_add 2
#define id_wc 1

void multiply(int *A, int A_row, int A_col, int *B, int B_row, int B_col, int *C, int C_row, int C_col);

int max(int data_len, int *data);

int add(int a, int b);

int wc(char *str, int str_len);

#endif
