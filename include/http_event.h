#ifndef __HTTP_EVENT_H__
#define __HTTP_EVENT_H__

#include <string>
#include "io_event.h"

class http_event: public io_event {
    public:
        /**
         * @brief construct
         *
         * @param svr
         */
        http_event(svr_base *svr);

        /**
         * @brief destruct
         */
        virtual ~http_event();

    public:
        /**
         * @brief get remote ip address
         *
         * @param ip
         */
        void set_ip(const std::string &ip) { m_ip = ip; }

        /**
         * @brief get remote port
         *
         * @param port
         */
        void set_port(unsigned short port) { m_port = port; }

        /**
         * @brief set current state
         *
         * @param state
         */
        void set_state(const std::string &state) { m_state = state; }

        /**
         * @brief get current state
         *
         * @return 
         */
        const std::string &get_state() { return m_state; }

        /**
         * @brief set response data
         *
         * @param resp
         */
        void set_response(const std::string &resp) { m_resp_data = resp; }

    public:

        /**
         * @brief get http head
         *
         * @return 
         */
        const std::string &get_head() { return m_head; }

        /**
         * @brief get http body
         *
         * @return 
         */
        const std::string &get_body() { return m_body; }

    public:
        /**
         * @brief notify incoming io event
         */
        void on_event();

    private:
        /**
         * @brief notify incoming read head event
         */
        void on_read_head();

        /**
         * @brief notify incoming read body event
         */
        void on_read_body();

        /**
         * @brief notify http packet complete event
         */
        void on_cmplt_pkg();

        /**
         * @brief notify incoming write event
         */
        void on_write();

    public:
        /**
         * @brief notify process event
         */
        virtual void on_process();

    private:
        /* address of the remote client */
        std::string m_ip;
        unsigned short m_port;

        /* io state */
        std::string m_state;

        /* http request data */
        int m_content_len;
        std::string m_head;
        std::string m_body;

        /* http response data */
        std::string m_resp_data;
};

#endif
