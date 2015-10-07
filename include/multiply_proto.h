#ifndef __MULTIPLY_PROTO__
#define __MULTIPLY_PROTO__

#include "basic_proto.h"

class multiply_proto : public basic_proto {
    public:
        multiply_proto(){
        }

        virtual ~multiply_proto(){
        
        }

    public:
        void set_A(int **A, int row_A, int col_A) {
            m_A = A;
            m_row_A = row_A;
            m_col_A = col_A;
        }

        void set_B(int **B) {
            m_B = B;
            m_row_B = row_B;
            m_col_B = col_B;
        }

        void set_C(int **C) {
            m_C = C;
            m_row_C = row_C;
            m_col_C = col_C;
        }

    protected:
        void add_matrix(int **data, int row, int col) {
            add_int(row);
            add_int(col);
            for (int i = 0; i < row; ++i) {
                add_binary(data[i], sizeof(int) * col);
            }
        }

        int read_matrix(int **&data, int &row, int &col) {
            if (read_int(row) < 0) {
                return -1;
            }
            if (read_int(col) < 0) {
                return -1;
            }
            data = (int*)malloc(sizeof(int*) * row);
            for (int i = 0; i < row; ++i) {
                if (read_binary(sizeof(int) * col, data[i]) < 0) {
                    return -1;
                }
            }
            return 0;
        }

    public:
        const char *encode(){
            encode_matrix(m_row_A, m_col_A, m_A);
            encode_matrix(m_row_B, m_col_B, m_B);
            encode_matrix(m_row_C, m_col_C, m_C);
            return m_buf.data();
        }

        void decode(const char *buf, int buf_len){
            read_matrix(m_row_A, m_col_A, m_A);
            read_matrix(m_row_B, m_col_B, m_B);
            read_matrix(m_row_C, m_col_C, m_C);
        }

    private:
        int **m_A;
        int m_row_A;
        int m_col_A;

        int **m_B;
        int m_row_B;
        int m_col_B;

        int **m_C;
        int m_row_C;
        int m_col_C;
};

#endif
