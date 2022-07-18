#include "network.h"
#include "connect_obj.h"
#include "packet.h"
#include "common.h"
#include "log4_help.h"
#include "object_pool.h"

void Network::Dispose() {
    Clean();
    ThreadObject::Dispose();
}

void Network::Clean() {
    for (auto iter = _connects.begin(); iter != _connects.end(); ++iter) {
        auto pObj = iter->second;
        pObj->Dispose();
    }
    _connects.clear();

    ::close(_epfd);

    //std::cout << "network dispose. close socket:" << _socket << std::endl;
    _sock_close(_masterSocket);
    _masterSocket = INVALID_SOCKET;
}

#define SetsockOptType const char *

void Network::SetSocketOpt(SOCKET socket) {
    // 1.端口关闭后马上重新启用
    bool isReuseaddr = true;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (SetsockOptType)& isReuseaddr, sizeof(isReuseaddr));

    // 2.发送、接收timeout
    int netTimeout = 3000; // 1000 = 1秒
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (SetsockOptType)& netTimeout, sizeof(netTimeout));
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (SetsockOptType)& netTimeout, sizeof(netTimeout));

    int keepAlive = 1;
    socklen_t optlen = sizeof(keepAlive);

    int keepIdle = 60 * 2;	// 在socket 没有交互后 多久 开始发送侦测包
    int keepInterval = 10;	// 多次发送侦测包之间的间隔
    int keepCount = 5;		// 侦测包个数

    setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (SetsockOptType)& keepAlive, optlen);
    if (getsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, &optlen) < 0) {
        LOG_DEBUG("getsockopt SO_KEEPALIVE failed.");
    }

    setsockopt(socket, SOL_TCP, TCP_KEEPIDLE, (void*)& keepIdle, optlen);
    if (getsockopt(socket, SOL_TCP, TCP_KEEPIDLE, &keepIdle, &optlen) < 0) {
        LOG_DEBUG("getsockopt TCP_KEEPIDLE failed.");
    }

    setsockopt(socket, SOL_TCP, TCP_KEEPINTVL, (void*)& keepInterval, optlen);
    setsockopt(socket, SOL_TCP, TCP_KEEPCNT, (void*)& keepCount, optlen);

    // 3.非阻塞
    _sock_nonblock(socket);
}

SOCKET Network::CreateSocket() {
    _sock_init();
    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket == INVALID_SOCKET) {
        LOG_DEBUG("::socket failed. err:" << _sock_err());
        return socket;
    }

    SetSocketOpt(socket);
    return socket;
}

void Network::CreateConnectObj(SOCKET socket) {
    ConnectObj* pConnectObj = DynamicObjectPool<ConnectObj>::GetInstance()->MallocObject(this, socket);

    if (_connects.find(socket) != _connects.end()) {
        LOG_DEBUG("Network::CreateConnectObj. socket is exist. socket:" << socket);
    }

    _connects[socket] = pConnectObj;

    AddEvent(_epfd, socket, EPOLLIN | EPOLLET | EPOLLRDHUP);
}

#define RemoveConnectObj(iter) \
    iter->second->Dispose( ); \
    DeleteEvent(_epfd, iter->first); \
    iter = _connects.erase( iter ); 

void Network::AddEvent(int epollfd, int fd, int flag) {
    struct epoll_event ev;
    ev.events = flag;
    ev.data.ptr = nullptr;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void Network::ModifyEvent(int epollfd, int fd, int flag) {
    struct epoll_event ev;
    ev.events = flag;
    ev.data.ptr = nullptr;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

void Network::DeleteEvent(int epollfd, int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr);
}

void Network::InitEpoll() {
    _epfd = epoll_create(MAX_CLIENT);
    AddEvent(_epfd, _masterSocket, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
}

void Network::Epoll() {
    _mainSocketEventIndex = -1;
    const int nfds = epoll_wait(_epfd, _events, MAX_EVENT, 0);
    for (int index = 0; index < nfds; index++) {
        int fd = _events[index].data.fd;

        if (fd == _masterSocket) {
            _mainSocketEventIndex = index;
        }

        auto iter = _connects.find(fd);
        if (iter == _connects.end()) {
            continue;
        }

        if (_events[index].events & EPOLLRDHUP || _events[index].events & EPOLLERR || _events[index].events & EPOLLHUP) {
            RemoveConnectObj(iter);
            continue;
        }

        if (_events[index].events & EPOLLIN) {
            if (!iter->second->Recv()) {
                RemoveConnectObj(iter);
                continue;
            }
        }

        if (_events[index].events & EPOLLOUT) {
            if (!iter->second->Send()) {
                RemoveConnectObj(iter);
                continue;
            }

            ModifyEvent(_epfd, iter->first, EPOLLIN | EPOLLRDHUP);
        }
    }

}

void Network::Update() {
    _sendMsgMutex.lock();
    if (_sendMsgList.CanSwap()) {
        _sendMsgList.Swap();
    }
    _sendMsgMutex.unlock();

    auto pList = _sendMsgList.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter) {
        Packet* pPacket = (*iter);
        auto socket = pPacket->GetSocket();
        auto itConnectObj = _connects.find(socket);
        if (itConnectObj == _connects.end()) {
            // TODO:
            LOG_DEBUG("send packet. can't find socket:" << socket);
            continue;
        }

        itConnectObj->second->SendPacket(pPacket);
        ModifyEvent(_epfd, socket, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
    }
    pList->clear();
}

void Network::HandleDisconnect(Packet* pPacket) {
    auto socket = pPacket->GetSocket();
    auto iter = _connects.find(socket);
    if (iter == _connects.end()) {
        LOG_DEBUG("dis connect failed. socket not find. socket:" << socket);
        return;
    }

    RemoveConnectObj(iter);
    LOG_DEBUG("logical layer requires shutdown. socket:" << socket);
}

void Network::SendPacket(Packet* pPacket) {
    std::lock_guard<std::mutex> guard(_sendMsgMutex);
    _sendMsgList.GetWriterCache()->emplace_back(pPacket);
}


