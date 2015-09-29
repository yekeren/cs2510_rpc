#include "add_proto.h"

void add_int(int x) {
    while (m_encoded_len + sizeof(x) > m_buf.size()) {
        m_buf.resize(m_buf.size() * 2);
    }
    *(int*)(m_buf.data() + m_encoded_len)  = x;
    m_encoded_len += sizeof(int);
}

void add_binary(char *bin, int bin_len) {
    while (m_encoded_len + bin_len > m_buf.size()) {
        m_buf.resize(m_buf.size() * 2);
    }
    memcpy(m_buf.data() + m_encoded_len, bin, bin_len);
    m_encoded_len += bin_len;
}



add_proto::add_proto() {
    m_encoded_len = 0;
    m_buf.resize(16);
}

add_proto::~add_proto() {
    if (m_buf) {
        free(m_buf);
    }
}

const char *matrix_proto::encode() {
    m_encoded_len = 0;

    add_int(m_a);
    add_int(m_b);
    add_int(m_retval);
    return m_buf.data();

    //add_binary(m_buf, m_encoded_len, m_A);
}

const char *add_proto::encode() {
    m_encoded_len = 0;

    add_int(m_a);
    add_int(m_b);
    add_int(m_retval);
    return m_buf.data();

    //add_binary(m_buf, m_encoded_len, m_A);
}

void add_proto::decode(const char *buf, int buf_len) {
    int ptr = 0;
    m_a = read_int(buf, ptr);
    m_b = read_int(buf, ptr);

    read_binary(buf, ptr, A);
    read_binary(buf, ptr, B);
    read_binary(buf, ptr, C);
    m_retval = read_int(buf, ptr);
}
