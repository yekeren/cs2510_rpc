#include "basic_proto.h"
#include <string.h>

basic_proto::basic_proto() {
    m_encoded_len = 0;
    m_buf.resize(16);
}

basic_proto::basic_proto(const char* buf, int buf_len){
    m_buf.assign(buf,buf_len);
    m_encoded_len = 0 ;
}

basic_proto::~basic_proto() {
}

void basic_proto::add_int(int retval){
    while (m_encoded_len + sizeof(retval) > m_buf.size()){
        m_buf.resize(m_buf.size()*2);
    }
    *(int*) (m_buf.data() + m_encoded_len) = retval;
    m_encoded_len += sizeof(int);
}

int basic_proto::read_int(int &retval){
    if ((m_encoded_len + sizeof(*retval)) > m_buf.size()){
        return -1;
    }
    retval = (int*)((char *)m_buf.data() + m_encoded_len);
    m_encoded_len += sizeof(*retval);
    return 1;
}

int basic_proto::read_binary(int bin_len, char* &bin){
    if((m_encoded_len + bin_len) > m_buf.size()){
        return -1;
    }
    m_buf.assign(bin, m_encoded_len, bin_len);
    
    m_encoded_len += bin_len;
    return 1;
}

void basic_proto::add_binary(const char * bin, int bin_len){
    while (m_encoded_len + bin_len >m_buf.size()){
        m_buf.resize(m_buf.size()*2);
    }
    memcpy((char*)m_buf.data() + m_encoded_len, bin, bin_len);
    m_encoded_len +=bin_len;
}

void basic_proto::add_array(int *data, int size) {
    add_int(size);
    add_binary(data, sizeof(int) * size);
}

int basic_proto::read_array(int *&data, int &size) {
    if (read_int(size) < 0) {
        return -1;
    }
    if (read_binary(size * sizeof(int)), data) {
        return -1;
    }
    return 0;
}

void basic_proto::add_matrix(int **data, int row, int col) {
    add_int(row);
    add_int(col);
    for (int i = 0; i < row; ++i) {
        add_binary(data[i], sizeof(int) * col);
    }
}

int basic_proto::read_matrix(int **&data, int &row, int &col) {
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

void basic_proto::add_string(int m_str_len, const char* str, int m_retval){
    //m_encoded_len = 0;
    add_int(m_str_len);
    add_binary(str, m_str_len);
    add_int(m_retval);
}

int basic_proto::read_string(int &m_str_len, char* &bin, int & m_retval){
    //m_encoded_len = 0;
    if(read_int(m_str_len) < 0){
        return -1;
    }
    if(read_binary(m_str_len, bin) <0){
        return -1;
    }
    if(read_int(m_retval) < 0){
        return -1;
    }
    return 0;
}

int basic_proto::get_buf_len(){
    return m_encoded_len;
}

const char* basic_proto::get_buf(){
    return m_buf.data();
}





