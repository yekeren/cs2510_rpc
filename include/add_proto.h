#ifndef __ADD_PROTO__
#define __ADD_PROTO__

class matrix_proto {
    public:
        set_A(int *A) {
            m_A = A;
        }

        set_m(int m) {
            m_m = m;
        }

        const char *encode() {
            add_binary(m_A, sizeof(int) * m_m * m_n);
            add_int(m_m);
            add_int(m_n);
            add_binary(m_B, sizeof(int) * m_m1 * m_n1);
            add_int(m_m1);
            add_int(m_n1);
        }
    private:
        int *m_A;
        int m_b;

        std::string m_buf;
        int m_encoded_len;
};

class add_proto {
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

        int get_buf_len() {
            return m_encoded_len;
        }

    public:
        const char *encode();

        void decode(const char *buf, int buf_len);

    private:
        int m_a;
        int m_b;

        std::string m_buf;
        int m_encoded_len;
};

#endif
