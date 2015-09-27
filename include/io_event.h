#ifndef __IO_EVENT_H__
#define __IO_EVENT_H__

#include <string>

class svr_base;

class io_event {
    public:
        io_event(svr_base *svr);
        virtual ~io_event();

    public:
        virtual void on_event();

    public:
        void set_fd(int fd) { m_fd = fd; }

        void set_io_type(char io_type) { m_io_type = io_type; }

    public:
        int get_fd() { return m_fd; }

        char get_io_type() { return m_io_type; }

    protected:
        svr_base *m_svr;

        int m_fd;
        char m_io_type;
};

class accept_event: public io_event {
    public:
        accept_event(svr_base *svr);
        virtual ~accept_event();

    public:
        int bind(unsigned short port, int backlog);

        void on_event();
};

class http_event: public io_event {
    public:
        http_event(svr_base *svr);
        virtual ~http_event();

    public:
        void set_ip(const std::string &ip) { m_ip = ip; }

        void set_port(unsigned short port) { m_port = port; }

        void set_state(const std::string &state) { m_state = state; }

        const std::string &get_state() { return m_state; }

        const std::string &get_head() { return m_head; }

        const std::string &get_body() { return m_body; }

    public:
        void on_event();

        void on_compute();

    private:
        void on_read_head();

        void on_read_body();

        void on_write();

        void callback();

    private:
        std::string m_ip;
        unsigned short m_port;

        std::string m_state;

        int m_content_len;
        std::string m_head;
        std::string m_body;

        std::string m_rsp;
};


#endif
