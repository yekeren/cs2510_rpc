#ifndef __ADD_PROTO__
#define __ADD_PROTO__

    public:
        set_A(int *A) {
            m_A = A;
        }

        set_m(int m) {
            m_m = m;

class add_proto : public basic_proto {
    public:
        add_proto();

        virtual ~add_proto();

    public:
        void set_a(int a) {
            m_a = a;
        }

        void set_b(int b) {
            m_b = b;
        }

        int get_a() {
            return m_a;
        }

        int get_b() {
            return m_b;
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
            basic_proto::add_int(m_a);
            basic_proto::add_int(m_b);
            basic_proto::add_int(m_retval);
            return m_buf.data();
        }

        void decode(const char *buf, int buf_len){
            int ptr = 0;
            if(ptr<buf_len){
                m_a = basic_proto::read_int(buf, ptr);
            }
            ptr+=sizeof(int);
            if(ptr<buf_len){
                m_b = basic_proto::read_int(buf, ptr);
            }
            ptr+=sizeof(int);
            if(ptr<buf_len){
                m_retval = basic_proto::read_int(buf, ptr);
            }
            ptr+=sizeof(int);
            
        }

    private:
        int m_a;
        int m_b;
        int m_retval;

        //std::string m_buf;
        //int m_encoded_len;
    
        //char *m_buf;
};

#endif
