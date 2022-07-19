
#include "common.h"
#include "network_connector.h"
#include "log4_help.h"
#include "global.h"

bool NetworkConnector::Init() {
    return true;
}

bool NetworkConnector::IsConnected() const {
    return _connects.size() > 0;
}

bool NetworkConnector::Connect(std::string ip, int port) {
    _ip = ip;
    _port = port;

    if (_port == 0 || _ip == "")
        return false;

    _masterSocket = CreateSocket();
    if (_masterSocket == INVALID_SOCKET)
        return false;

    InitEpoll();

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    ::inet_pton(AF_INET, _ip.c_str(), &addr.sin_addr.s_addr);

    int rs = ::connect(_masterSocket, (struct sockaddr *)&addr, sizeof(sockaddr));
    if (rs == 0) {
        // 成功
        CreateConnectObj(_masterSocket);
    }

    return true;
}

void NetworkConnector::TryCreateConnectObj() {
    int optval = -1;
    socklen_t optlen = sizeof(optval);
    int rs = ::getsockopt(_masterSocket, SOL_SOCKET, SO_ERROR, (char*)(&optval), &optlen);
    if (rs == 0 && optval == 0) {
        CreateConnectObj(_masterSocket);
    } else {
        LOG_DEBUG("connect failed. socket:" << _masterSocket);
        Clean();
    }
}

void NetworkConnector::Update() {
    // 如果断线，重新连接
    if (_masterSocket == INVALID_SOCKET) {
        if (!Connect(_ip, _port))
            return;

        LOG_DEBUG("re connect. socket:" << _masterSocket);
    }

    Epoll();

    if (!IsConnected())  {
        if (_mainSocketEventIndex >= 0) {
            int fd = _events[_mainSocketEventIndex].data.fd;
            if (fd != _masterSocket)
                return;

            if (_nextConnectTime > Global::GetInstance()->TimeTick)
                return;

            _nextConnectTime = timeutil::AddSeconds(Global::GetInstance()->TimeTick, 5);
            // connect成功，会触发IN事件
            if (_events[_mainSocketEventIndex].events & EPOLLIN || _events[_mainSocketEventIndex].events & EPOLLOUT) {
                TryCreateConnectObj();
            }

        }
    }

    Network::Update();
}
