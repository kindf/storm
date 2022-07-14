#pragma once

#include <thread>

#include "sn_object.h"
#include "disposable.h"
#include "cache_swap.h"
#include "thread_obj.h"
#include "packet.h"
#include "cache_refresh.h"

enum ThreadState {
    ThreadState_Init,
    ThreadState_Run,
    ThreadState_Stoped,
};

class Thread : public SnObject, public IDisposable {
public:
    Thread();
    void Start();
    bool IsRun() const;
    bool IsStop() const;
    bool IsDispose();
    void Dispose() override;

public:
    void Update();
    void AddPacketToList(Packet* pPacket);
    void AddObject(ThreadObject* pThreadObj);

private:
    ThreadState _state;
    std::thread _thread;

    std::mutex _objs_lock;
    CacheRefresh<ThreadObject> _objList;
    // 本线程中的所有待处理包
    std::mutex _packet_lock;
    CacheSwap<Packet> _cachePackets;
};

