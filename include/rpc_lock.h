#ifndef __RPC_LOCK_H__
#define __RPC_LOCK_H__

#ifdef __APPLE__
#include <libkern/OSAtomic.h>
#elif __linux
#include <pthread.h>
#endif

class spin_lock {
    public:
        spin_lock() { }
        virtual ~spin_lock() { }

    public:
        int init() {
#ifdef __APPLE__
            m_lock = OS_SPINLOCK_INIT;
            return 0;
#elif __linux
            return pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE);
#endif
        }

        int lock() {
#ifdef __APPLE__
            OSSpinLockLock(&m_lock);
            return 0;
#elif __linux
            return pthread_spin_lock(&m_lock);
#endif
        }

        int unlock() {
#ifdef __APPLE__
            OSSpinLockUnlock(&m_lock);
            return 0;
#elif __linux
            return pthread_spin_unlock(&m_lock);
#endif
        }

        void destroy() {
#ifdef __linux
            pthread_spin_destroy(&m_lock);
#endif
        }

    private:
#ifdef __APPLE__
        OSSpinLock m_lock;
#elif __linux
        pthread_spinlock_t m_lock;
#endif
};

#endif
