#ifndef __IO_EVENT_H__
#define __IO_EVENT_H__

class svr_base;

class io_event {
    public:
        /**
         * @brief construct
         *
         * @param svr
         */
        io_event(svr_base *svr);

        /**
         * @brief destruct
         */
        virtual ~io_event();

    public:
        /**
         * @brief notify incoming io event
         */
        virtual void on_event();

    public:
        /**
         * @brief set file descriptor
         *
         * @param fd
         */
        void set_fd(int fd) { m_fd = fd; }

        /**
         * @brief set io type
         *
         * @param io_type
         */
        void set_io_type(char io_type) { m_io_type = io_type; }

        /**
         * @brief get file descriptor
         *
         * @return 
         */
        int get_fd() { return m_fd; }

        /**
         * @brief get io type
         *
         * @return 
         */
        char get_io_type() { return m_io_type; }

    public:
        /**
         * @brief add reference
         */
        void add_ref();

        /**
         * @brief release reference, delete if it equals 0
         */
        void release();


    protected:
        svr_base *m_svr;

        int m_fd;
        char m_io_type;
        int m_ref;
};


#endif
