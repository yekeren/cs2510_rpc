#include "io_event.h"
#include <stdlib.h>
#include "rpc_net.h"
#include "rpc_log.h"
#include "svr_base.h"

/**************************************
 * io_event
 **************************************/
/**
 * @brief construct
 *
 * @param svr
 */
io_event::io_event(svr_base *svr): 
    m_svr(svr), 
    m_fd(-1), 
    m_io_type('i'),
    m_timeout_ms(0) { 

#ifdef __APPLE__
    m_ref = 0;
#elif __linux
    atomic_set(&m_ref, 0);
#endif
}

/**
 * @brief destruct
 */
io_event::~io_event() { 
    if (m_fd != -1) {
        close(m_fd);
        RPC_DEBUG("close fd, fd=%d", m_fd);
    }
    m_fd = -1;
}

/**
 * @brief notify incoming io event
 */
void io_event::on_event() { 
    RPC_DEBUG("on_event");
}

/**
 * @brief notify process event(multi-threads call)
 */
void io_event::on_process() {
    RPC_DEBUG("on_process");
}

/**
 * @brief notify timeout event
 */
void io_event::on_timeout() {
    RPC_DEBUG("on_timeout");
}

/**
 * @brief add reference
 */
void io_event::add_ref() {
#ifdef __APPLE__
    ++m_ref;
#elif __linux
    atomic_inc(&m_ref);
#endif
}

/**
 * @brief release reference, delete if it equals 0
 */
void io_event::release() {
#ifdef __APPLE__
    if (--m_ref == 0) {
        delete this;
    }
#elif __linux
    if (atomic_dec_and_test(&m_ref)) {
        delete this;
    }
#endif
}
