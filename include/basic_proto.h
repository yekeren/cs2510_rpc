//
//  basic_proto.h
//  
//
//  Created by Xinyue Huang on 10/1/15.
//
//

#ifndef _basic_protoco_h
#define _basic_protoco_h

#include <string>

class basic_proto{
    public:
        basic_proto();
        virtual ~basic_proto();
    
    public:
        void add_int(int x);
        int read_int (const char* buf, int & retval);
        int read_binary(const char *buf, int buf_len, char* &data);
        void add_binary(const char* bin, int bin_len);
    
    
    protected:
        int m_encoded_len;
        std::string m_buf;
};
#endif
