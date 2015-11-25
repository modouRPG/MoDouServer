#include "MapServer.h"
#include <iostream>
#include <fcntl.h>
#include <netinet/tcp.h>
#include "Session.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../net/pkg.h"

//uint8_t buf[1024];

namespace modou
{
    MapServer::MapServer(string ip, int port) : mPort(port)
    {
        if (inet_aton(ip.c_str(), &mAddr) == 0) {
            cerr << "Invalid ip addr." << endl;
            return;
        }
        mMaxClient = 1024;
        mEpoll = epoll_create(mMaxClient);
    }

    MapServer::~MapServer()
    {}

    void MapServer::start()
    {
        init();
        while(1) {
            sendrecv();
            //parse();
        }
    }

    void MapServer::init()
    {
        int yes = 1;
        int flag = fcntl(mSock, F_GETFL, 0);
        struct sockaddr_in addr;

        mSock = socket(AF_INET, SOCK_STREAM, 0);
        fcntl(mSock, F_SETFL, flag | O_NONBLOCK);
        setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        setsockopt(mSock, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int));

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(mPort);

        bind(mSock, (struct sockaddr *)&addr, sizeof(struct sockaddr));
        listen(mSock, mMaxClient);

        struct epoll_event ev;
        Session *session = new Session();
        session->mSocket = mSock;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = session;
        epoll_ctl(mEpoll, EPOLL_CTL_ADD, mSock, &ev);
    }

    void MapServer::sendrecv()
    {
        int nfds, uSock;
        struct sockaddr_in peer;
        struct epoll_event ev;
        socklen_t len = sizeof(peer);
        Session *s1, *s2;
        int flag = fcntl(mSock, F_GETFL, 0);
        uint32_t data_len = 0;

        nfds = epoll_wait(mEpoll, events, EVENT_LEN, -1);
        for(int i=0 ;i<nfds; i++) {
            s1 = (Session *)(events[i].data.ptr);
            if (s1->mSocket == mSock) {
                uSock = accept(mSock, (struct sockaddr *)&peer, &len);
                fcntl(uSock, F_SETFL, flag | O_NONBLOCK);
                ev.events = EPOLLIN | EPOLLET;
                cout << inet_ntoa(peer.sin_addr)  << " connected." << endl;
                // init Session.
                s2 = new Session();
                s2->mSocket = uSock;
                s2->in_buf = (uint8_t *)calloc(IN_BUF_LEN, sizeof(uint8_t));
                s2->in_data_len = 0;
                s2->in_size = IN_BUF_LEN;
                s2->out_buf = (uint8_t *)calloc(OUT_BUF_LEN, sizeof(uint8_t));
                s2->out_data_len = 0;
                s2->out_size = OUT_BUF_LEN;
                s2->mAddr = peer.sin_addr;
		users.push_back(s2);
                // end init
                ev.data.ptr = s2;
                epoll_ctl(mEpoll, EPOLL_CTL_ADD, uSock, &ev);
            } else {
                if (events[i].events & EPOLLIN) {
                    cout << "in " << inet_ntoa(s1->mAddr) << endl;
                    //data_len = read(s1->mSocket, s1->in_buf + s1->in_data_len, s1->in_size - s1->in_data_len);
                    data_len = recv(s1->mSocket, s1->in_buf + s1->in_data_len, s1->in_size - s1->in_data_len, 0);
                    if (data_len > 0) {
                        s1->in_data_len += data_len;
                    } else if (data_len == 0 ) {
		      return;
		    } else {
                        cout << "End of Conn. " << inet_ntoa(s1->mAddr) << endl;
                        s1->eof = 1;
                        epoll_ctl(mEpoll, EPOLL_CTL_DEL, s1->mSocket, NULL);
                        Session::clear(s1);
                        return;
                    }
                    parse(s1);
                    ev.events = EPOLLOUT | EPOLLET;
                    ev.data.ptr = s1;
                    epoll_ctl(mEpoll, EPOLL_CTL_MOD, s1->mSocket, &ev);
                } else if (events[i].events & EPOLLOUT) {
                    cout << "out " << inet_ntoa(s1->mAddr) << endl;
                    //write(s1->mSocket, "hello", sizeof("hello"));
                    //data_len = write(s1->mSocket, s1->out_buf, s1->out_data_len);
                    data_len = send(s1->mSocket, s1->out_buf, s1->out_data_len, 0);
                    cout << "Send Hello. "  << data_len << endl;
                    if (data_len > 0) {
                        //TODO: 
                        s1->out_data_len -= data_len;
                        memset(s1->out_buf, 0, data_len);
                    } else if (data_len == 0) {
		      return;
		    }
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.ptr = s1;
                    epoll_ctl(mEpoll, EPOLL_CTL_MOD, s1->mSocket, &ev);
                }
            }
        }
    }

    void MapServer::parse(Session *sess)
    {
      uint8_t flag, ret;
      
      if (sess->eof)  {
	return;
      }
      flag = GET_FLAG(sess);
      std::cout << flag << std::endl;
      switch(flag) {
      case SAY_TO_NPC_FLAG:
	//say_to_npc_pkg *pkg = (say_to_npc_pkg *)GET_DATA(sess);
	
	break;
      case GET_NPC_BY_MAP_POS:
	std::cout << " get npc by map pos pkg " << std::endl;
	get_npc_by_map_pos_pkg *pkg = (get_npc_by_map_pos_pkg *)GET_DATA(sess);

	npcs_list_pkg *pkg2 = (npcs_list_pkg *)calloc(1, sizeof(npcs_list_pkg));
	pkg2->flag = NPCS_LIST;
	pkg2->num = 1;

	npc_info *npc = (npc_info *)calloc(1, sizeof(npc_info));
	strcpy(npc->name, "老奶奶");
	npc->picId = 1;
	npc->posX = 544;
	npc->posY = 288;
	npc->width = 60;
	npc->height = 60;

	memcpy(sess->out_buf + sess->out_data_len, pkg2, sizeof(npcs_list_pkg));
	sess->out_data_len += sizeof(npcs_list_pkg);
	memcpy(sess->out_buf + sess->out_data_len, npc, sizeof(npc_info));
	sess->out_data_len += sizeof(npc_info);

	std::cout << " send npcs end " << std::endl;
	
	free(pkg2);
	free(npc);
	break;
      }
    }
}
