#ifndef __SVR_BASE__
#define __SVR_BASE__

#include <string>
#include <list>
#include "rpc_lock.h"
#include "svr_thrd.h"
#include "io_event.h"

class svr_base {
    public:
        /**
         * @brief construct
         */
        svr_base();

        /**
         * @brief destruct
         */
        virtual ~svr_base();

    public:
        /**
         * @brief create process threads
         *
         * @param thrds_num
         *
         * @return 
         */
        int run(int thrds_num = 1);

        /**
         * @brief stop process threads
         */
        void stop();

        /**
         * @brief join process threads
         */
        void join();

    public:
        /**
         * @brief bind to specific port
         *
         * @param port
         * @param backlog
         *
         * @return 
         */
        int bind(unsigned short port, int backlog = 32);

        /**
         * @brief routint
         *
         * @param timeout_ms
         */
        void run_routine(int timeout_ms = 10);

    public:
        /**
         * @brief create event
         *
         * @param fd
         * @param ip
         * @param port
         *
         * @return 
         */
        virtual io_event *create_event(int fd,
                const std::string &ip, unsigned short port);

        /**
         * @brief dispatch task into thread
         *
         * @param evt
         */
        virtual void on_dispatch_task(io_event *evt);

        /**
         * @brief add io event to select queue
         *
         * @param evt
         */
        void add_io_event(io_event* evt);

    private:
        std::vector<svr_thrd*> m_thrds_pool;

        std::list<io_event*> m_evts;
        std::list<io_event*> m_evts_appd;

        spin_lock m_lock;
};

#endif
