#ifndef __SVR_THRD__
#define __SVR_THRD__

#include <stdInt.h>
#include <string>

class svr_thrd_base {
    public:
        svr_thrd_base();
        virtual ~svr_thrd_base();

    public:
        int run();

        virtual void stop() { }

        virtual void join() { }

        virtual bool is_running() { return false; }

    public:
        static void *run_routine(void *args);

        virtual void run_routine();

        virtual void proc_new_conn(int fd, 
                const std::string &ip, uint16_t port);

    private:
        int m_fd;
        std::string m_ip;
        uint16_t m_port;
};

class svr_thrd_mgr_base {
    public:
        svr_thrd_mgr_base();
        virtual ~svr_thrd_mgr_base();

    public:
        virtual void proc_new_conn(int fd, 
                const std::string &ip, uint16_t port);
};

#endif
