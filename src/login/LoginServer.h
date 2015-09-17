#ifndef _SERVER_CPP_
#define _SERVER_CPP_

#include <string>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <vector>
#include "main.h"
#include "Session.h"

using namespace std;

namespace modou
{
    class LoginServer
    {
        public:
            LoginServer(string ip, int port);

            virtual ~LoginServer();

            void start();
            
        private:
            void init();
            void sendrecv();
            void parse(Session *sess);
            uint8_t auth(string email, string pass);

            struct in_addr mAddr;
            struct epoll_event events[EVENT_LEN];

            vector< Session* > users;

            int mPort;
            int mSock;
            int mEpoll;
            int mMaxClient;
    };
}

#endif
