#include "basic_proto.h"
#include <string.h>

basic_proto::basic_proto() {
    m_encoded_len = 0;
    m_buf.resize(16);
}

basic_proto::~basic_proto() {
}

void basic_proto::add_int(int x){
    while (m_encoded_len + sizeof(x) > m_buf.size()){
        m_buf.resize(m_buf.size()*2);
    }
    *(int*) (m_buf.data() + m_encoded_len) = x;
    m_encoded_len += sizeof(int);
}

int basic_proto::read_int(const char* buf, int offset){
    return *(int*) (buf+offset);
}

char* basic_proto::read_binary(const char *buf, int offset){
    return (char*)(buf+offset);
}

void basic_proto::add_binary(const char * bin, int bin_len){
    while (m_encoded_len + bin_len >m_buf.size()){
        m_buf.resize(m_buf.size()*2);
    }
    memcpy((char*)m_buf.data() + m_encoded_len, bin, bin_len);
    m_encoded_len +=bin_len;
}


