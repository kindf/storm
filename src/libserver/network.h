#pragma once

#include <map>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include "common.h"
#include "thread_obj.h"
#include "socket_object.h"
#include "cache_swap.h"

#define _sock_init( )
#define _sock_nonblock( sockfd ) { int flags = fcntl(sockfd, F_GETFL, 0); fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); }
#define _sock_exit( )
#define _sock_err( )	errno
#define _sock_close( sockfd ) ::shutdown( sockfd, SHUT_RDWR ) 
#define _sock_is_blocked()	(errno == EAGAIN || errno == 0)

class ConnectObj;
class Packet;

class Network : public ThreadObject, public ISocketObject {
public:
    void Dispose() override;
    SOCKET GetSocket() override { return _masterSocket; }
    void SendPacket(Packet*);

protected:
    static void SetSocketOpt(SOCKET socket);
    static SOCKET CreateSocket();
    void CreateConnectObj(SOCKET socket);
    void Clean();

    void InitEpoll();
    void Epoll();
    void AddEvent(int epollfd, int fd, int flag);
    void ModifyEvent(int epollfd, int fd, int flag);
    void DeleteEvent(int epollfd, int fd);
    void Update() override;

private:
    void HandleDisconnect(Packet* pPacket);

protected:
    SOCKET _masterSocket{ INVALID_SOCKET };
    std::map<SOCKET, ConnectObj*> _connects;

#define MAX_CLIENT  5120
#define MAX_EVENT   5120
    struct epoll_event _events[MAX_EVENT];
    int _epfd;
    int _mainSocketEventIndex{ -1 };
    // ∑¢ÀÕ–≠“È
    std::mutex _sendMsgMutex;
    CacheSwap<Packet> _sendMsgList;
};
