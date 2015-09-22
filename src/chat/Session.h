#ifndef _SESSION_H_
#define _SESSION_H_
#include <stdint.h>
#include <arpa/inet.h>

namespace modou
{
    class Session
    {
        public:
            Session();
            virtual ~Session();

            static void clear(Session *s);

            struct in_addr mAddr;

            int mSocket;

            uint8_t *in_buf;
            uint8_t *out_buf;

            uint32_t in_pos;
            uint32_t in_data_len;
            uint32_t in_data_pos;
            uint32_t in_size;
            uint32_t out_pos;
            uint32_t out_data_len;
            uint32_t out_size;

            uint8_t eof;
    };
}

#endif
