#ifndef __ADD_PROTO__
#define __ADD_PROTO__

class add_proto {
    public:
        add_proto() {
        }

        virtual ~add_proto() {
        }

    public:
        void set_a(int a) {
            *(int*)(m_buf + 0) = a;
        }

        void set_b(int b) {
            *(int*)(m_buf + 4) = b;
        }

        void set0_ret(int ret) {
            *(int*)(m_buf + 8) = ret;
        }

        int get_a() {
            return *(int*)(m_buf + 0);
        }

        int get_b() {
            return *(int*)(m_buf + 4);
        }

        int get0_ret() {
            return *(int*)(m_buf + 8);
        }

    public:
        void create_buffer(int len) {
        }

    public:
        void attach_buf(char *buf, int buf_len) {
            memcpy(m_buf, buf, buf_len);
        }

        void detach_buf(char *buf, int buf_len) {
            memcpy(buf, m_buf, buf_len);
        }

    private:
        char *m_buf;
};

#endif
