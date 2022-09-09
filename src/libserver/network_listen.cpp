#include "common.h"
#include "network_listen.h"
#include "connect_obj.h"
#include "log4_help.h"

bool NetworkListen::Init() {
    return true;
}

bool NetworkListen::Listen(std::string ip, int port) {
    _masterSocket = CreateSocket();
    if (_masterSocket == INVALID_SOCKET)
        return false;

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    ::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

    if (::bind(_masterSocket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        LOG_WARN("::bind failed. err:" << _sock_err());
        return false;
    }

    if (::listen(_masterSocket, SOMAXCONN) < 0) {
        LOG_WARN("::listen failed." << _sock_err());
        return false;
    }

    InitEpoll();
    return true;
}

int NetworkListen::Accept() {
    struct sockaddr socketClient;
    socklen_t socketLength = sizeof(socketClient);

    int rs = 0;
    while (true) {
        const SOCKET socket = ::accept(_masterSocket, &socketClient, &socketLength);
        if (socket == INVALID_SOCKET)
            return rs;

        SetSocketOpt(socket);
        CreateConnectObj(socket);

        ++rs;
    }
}

void NetworkListen::Update() {
    Epoll();

    if (_mainSocketEventIndex >= 0) {
        Accept();
    }

    Network::Update();
}

void NetworkListen::ProcessPacket(Packet* pPacket) {
    LOG_DEBUG("[NetworkListen::ProcessPacket]");
}

