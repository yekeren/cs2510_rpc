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
    m_ref(0){ 
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
 * @brief add reference
 */
void io_event::add_ref() {
    ++m_ref;
}

/**
 * @brief release reference, delete if it equals 0
 */
void io_event::release() {
    if (--m_ref == 0) {
        delete this;
    }
}
