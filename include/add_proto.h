
class add_marshal {
    public:
        set_a(int a) {
            *(int*)&buffer[pt] = a;
            pt += sizeof(int);
            xmlObj.addint();
        }
        set_b(int b);

        const char *get_send_buffer() {
            return buffer;
        }

        int get_send_buffer_len() {
            return pt;
        }

        int decode(void *buf, int buf_len) {
            this->a = *(int*)buf;
        }

    private:
        int a, b;
        int pt;
        char buffer[1024];
};
