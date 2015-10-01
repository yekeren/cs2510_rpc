#include "add_proto.h"
#include "basic_proto.h"

add_proto::add_proto() {
    m_encoded_len = 0;
    m_buf.resize(16);
}

add_proto::~add_proto() {
}

