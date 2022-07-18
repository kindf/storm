#include "connect_obj.h"
#include "network.h"
#include "network_buffer.h"
#include "packet.h"
#include "thread_mgr.h"
#include "object_pool_interface.h"
#include "log4_help.h"

ConnectObj::ConnectObj(IDynamicObjectPool* pPool) :ObjectBlock(pPool) {
    _pNetWork = nullptr;
    _socket = INVALID_SOCKET;
    _recvBuffer = new RecvNetworkBuffer(DEFAULT_RECV_BUFFER_SIZE, this);
    _sendBuffer = new SendNetworkBuffer(DEFAULT_SEND_BUFFER_SIZE, this);
}

ConnectObj::~ConnectObj() {
    if (_recvBuffer != nullptr)
        delete _recvBuffer;

    if (_sendBuffer != nullptr)
        delete _sendBuffer;
}

void ConnectObj::TakeoutFromPool(Network* pNetWork, SOCKET socket) {
    _pNetWork = pNetWork;
    _socket = socket;
}

void ConnectObj::BackToPool() {
    if (!Global::GetInstance()->IsStop) {
        // TODO:
        // 通知其他对象，有Socket中断了
        /* Packet* pResultPacket = new Packet(Proto::MsgId::MI_NetworkDisconnect, _socket); */
        /* MessageList::DispatchPacket(pResultPacket); */
    }

    _pNetWork = nullptr;
    _socket = INVALID_SOCKET;
    _recvBuffer->BackToPool();
    _sendBuffer->BackToPool();

    _pPool->FreeObject(this);
}

bool ConnectObj::HasRecvData() const {
    return _recvBuffer->HasData();
}

Packet* ConnectObj::GetRecvPacket() const {
    return _recvBuffer->GetPacket();
}

bool ConnectObj::Recv() const {
    bool isRs = false;
    char* pBuffer = nullptr;
    while (true) {
        // 总空间数据不足一个头的大小，扩容
        if (_recvBuffer->GetEmptySize() < (sizeof(PacketHead) + sizeof(TotalSizeType))) {
            _recvBuffer->ReAllocBuffer();
        }

        const int emptySize = _recvBuffer->GetBuffer(pBuffer);
        const int dataSize = ::recv(_socket, pBuffer, emptySize, 0);
        if (dataSize > 0) {
            _recvBuffer->FillDate(dataSize);
        } else if (dataSize == 0) {
            break;
        } else {
            const auto socketError = _sock_err();
            if (socketError == EINTR || socketError == EWOULDBLOCK || socketError == EAGAIN) {
                isRs = true;
            }
            break;
        }
    }

    if (isRs) {
        while (true) {
            const auto pPacket = _recvBuffer->GetPacket();
            if (pPacket == nullptr)
                break;

            // TODO:
            /* if (_pNetWork->IsBroadcast() && _pNetWork->GetThread() != nullptr) { */
            /*     ThreadMgr::GetInstance()->DispatchPacket(pPacket); */
            /* } else { */
            /*     _pNetWork->GetThread()->AddPacketToList(pPacket); */
            /* } */
        }
    }

    return isRs;
}

bool ConnectObj::HasSendData() const {
    return _sendBuffer->HasData();
}

void ConnectObj::SendPacket(Packet* pPacket) const {
    _sendBuffer->AddPacket(pPacket);
}

bool ConnectObj::Send() const {
    while (true) {
        char* pBuffer = nullptr;
        const int needSendSize = _sendBuffer->GetBuffer(pBuffer);

        // 没有数据可发送
        if (needSendSize <= 0) {
            return true;
        }

        const int size = ::send(_socket, pBuffer, needSendSize, 0);
        if (size > 0) {
            _sendBuffer->RemoveDate(size);

            // 下一帧再发送
            if (size < needSendSize) {
                return true;
            }
        }

        if (size <= 0) {
            const auto socketError = _sock_err();
            LOG_WARN("needSendSize:" << needSendSize << " error:" << socketError);
            return false;
        }
    }
}

void ConnectObj::Close() {
    // TODO:
    /* const auto pPacketDis = new Packet(Proto::MsgId::MI_NetworkDisconnectEx, GetSocket()); */
    /* _pNetWork->GetThread()->AddPacketToList(pPacketDis); */
}

