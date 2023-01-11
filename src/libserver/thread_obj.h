#pragma once

#include "sn_object.h"
#include "disposable.h"
#include "cache_swap.h"
#include "common.h"

class Thread;
class Packet;
class ThreadObject : public SnObject, public IDisposable {
public:
    ThreadObject(int obj_type = TOT_DEFAULT) :_active(true), _pThread(nullptr), _obj_type(obj_type){};
    virtual bool Init() = 0;
    virtual void Update() = 0;
    virtual void ProcessPacket(Packet* pPacket){};

    void SetThread(Thread* pThread);
    Thread* GetThread() const;
    bool IsActive() const;
    void Dispose() override;
    void SetObjType(int type);
    int GetObjType() const;

    std::list<Packet*>* GetPackets();
    void AddPacket(Packet* pPacket);

protected:
    bool _active;
    Thread* _pThread;
    int _obj_type;

    // 本obj中的所有待处理包
    std::mutex _packet_lock;
    CacheSwap<Packet> _cachePackets;
};
