#pragma once

#include <thread>

#include "sn_object.h"
#include "disposable.h"
#include "cache_swap.h"
#include "thread_obj.h"
#include "packet.h"

enum ThreadState {
    ThreadState_Init,
    ThreadState_Run,
    ThreadState_Stoped,
};

class Thread : public SnObject, public IDisposable {
public:
    Thread(ThreadObject* pThreadObj);
    void Start();
    bool IsRun() const;
    bool IsStop() const;
    bool IsDispose();
    void Dispose() override;

public:
    void Update();
    void AddPacketToList(Packet* pPacket);

private:
    ThreadState _state;
    std::thread _thread;

    ThreadObject* _threadObj;
    // 本线程中的所有待处理包
    std::mutex _packet_lock;
    CacheSwap<Packet> _cachePackets;
};

