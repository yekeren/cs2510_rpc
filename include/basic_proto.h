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
        void add_int(int retval);
        int read_int (int & retval);

        void add_binary(const char* bin, int bin_len);
        int read_binary(int bin_len, char* &bin);

        void add_matrix(int **data, int row, int col);
        int read_matrix(int **&data, int &row, int &col);
    
    protected:
        int m_encoded_len;
        std::string m_buf;
};
#endif
