#include "basic_proto.h"
#include <string.h>

basic_proto::basic_proto() {
    m_encoded_len = 0;
    m_buf.resize(16);
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
