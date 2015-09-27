#ifndef __SVR_THRD__
#define __SVR_THRD__

#include <pthread.h>
#include <vector>
#include <list>

class http_event;

class svr_thrd {
    public:
        /**
         * @brief construct
         */
        svr_thrd();

        /**
         * @brief distruct
         */
        virtual ~svr_thrd();

    public:
        /**
         * @brief create computing thread
         *
         * @return 
         */
        int run();

        /**
         * @brief return the status the this thread
         *
         * @return 
         */
        virtual bool is_running() { return m_running; }

        /**
         * @brief stop computing thread
         */
        virtual void stop() { m_running = false; }

        /**
         * @brief join this thread
         */
        virtual void join();

    public:
        /**
         * @brief add a computing task
         *
         * @param evt
         */
        virtual void add_task(http_event *evt);

    private:
        /**
         * @brief 
         *
         * @param args
         *
         * @return 
         */
        static void *run_routine(void *args);

        /**
         * @brief 
         */
        virtual void run_routine();

    private:
        bool m_running;
        pthread_t m_thrd_id;

        pthread_cond_t m_cond;
        pthread_mutex_t m_mutex;
        std::list<http_event*> m_tasks;
};

class svr_thrd_dsptch {
    public:
        /**
         * @brief construct 
         */
        svr_thrd_dsptch();

        /**
         * @brief distruct
         */
        virtual ~svr_thrd_dsptch();

    public:
        /**
         * @brief dispatch computing task
         *
         * @param evt
         */
        virtual void dispatch_task(http_event *evt);

        /**
         * @brief set computing threads
         *
         * @param thrds
         */
        void set_thrds(std::vector<svr_thrd*> &thrds) { 
            m_thrds = thrds;
        }

    private:
        std::vector<svr_thrd*> m_thrds;
};

#endif
