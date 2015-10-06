#ifndef __WC_PROTO__
#define __WC_PROTO__

#include "basic_proto.h"

class wc_proto : public basic_proto {
    public:
        wc_proto(){
            m_encoded_len = 0;
            m_buf.resize(16);
        }

        virtual ~wc_proto(){
        
        }

    public:
        void set_s(std::string a) {
            m_str = a;
            m_str_len = a.length();
        }
    
        void set_retval(int val){
            m_retval = val;
        }
    
        int get_str_len(){
            return m_str_len;
        }
    
        std::string get_str(){
            return m_str;
        }
    
        int get_retval(){
            return m_retval;
        }

        int get_buf_len() {
            return m_encoded_len;
        }

    public:
        const char *encode(){
            m_encoded_len = 0;
            basic_proto::add_int(m_str_len);
            basic_proto::add_binary(m_str.c_str(), m_str_len);
            basic_proto::add_int(m_retval);
            return m_buf.data();
        }

        void decode(const char *buf, int buf_len){
            int ptr = 0;
            if(ptr<buf_len){
                m_str_len = basic_proto::read_int(buf, ptr);
            }
            ptr+=sizeof(int);
            if(ptr<buf_len){
                m_str = basic_proto::read_binary(buf, ptr);
            }
            ptr+=sizeof(char)*m_str_len;
            if(ptr<buf_len){
                m_retval = basic_proto::read_int(buf, ptr);
            }
            ptr+=sizeof(int);
            
        }

    private:
        std::string m_str;
        int m_str_len;
        int m_retval;

        //std::string m_buf;
        //int m_encoded_len;
    
        //char *m_buf;
};

#endif
