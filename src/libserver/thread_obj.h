#pragma once

#include "sn_object.h"
#include "disposable.h"
#include "common.h"

class Thread;
class Packet;
class ThreadObject : public SnObject, public IDisposable {
public:

    virtual bool Init() = 0;
    virtual void Update() = 0;    
    virtual void ProcessPacket(Packet* pPacket){};

    void SetThread(Thread* pThread);
    Thread* GetThread() const;
    bool IsActive() const;
    void Dispose() override;
    bool CheckObjType(int type) const;
    int GetObjType() const;   

protected:
    bool _active{ true };
    Thread* _pThread{ nullptr };
    int _obj_type{ TOT_DEFAULT };
};
