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
    m_ref(0),
    m_timeout_ms(0) { 

#ifdef __APPLE__
    pthread_mutex_init(&m_lock, NULL);
#elif __linux
    pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE);
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

#ifdef __APPLE__
    pthread_mutex_destroy(&m_lock);
#elif __linux
    pthread_spin_destroy(&m_lock);
#endif
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
    pthread_mutex_lock(&m_lock);
#elif __linux
    pthread_spin_lock(&m_lock);
#endif

    ++m_ref;

#ifdef __APPLE__
    pthread_mutex_unlock(&m_lock);
#elif __linux
    pthread_spin_unlock(&m_lock);
#endif
}

/**
 * @brief release reference, delete if it equals 0
 */
void io_event::release() {

#ifdef __APPLE__
    pthread_mutex_lock(&m_lock);
#elif __linux
    pthread_spin_lock(&m_lock);
#endif

    if (--m_ref == 0) {
        delete this;
        return;
    }

#ifdef __APPLE__
    pthread_mutex_unlock(&m_lock);
#elif __linux
    pthread_spin_unlock(&m_lock);
#endif
}
