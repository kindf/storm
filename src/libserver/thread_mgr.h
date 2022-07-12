#pragma once

#include <mutex>

#include "common.h"
#include "thread.h"
#include "singleton.h"
#include <map>

class Packet;
class ThreadObject;

enum THREA_TYPE {
    TT_CLIENT = 0,
    TT_WORLD,
    TT_LOGIN,
    TT_DATABASE,
};

class ThreadMgr :public Singleton<ThreadMgr> {
public:
    ThreadMgr();
    void StartAllThread();

    bool IsStopAll();
    bool IsDisposeAll();
    void Dispose() override;    

    bool AddObjToThread(THREA_TYPE threadType, ThreadObject* obj);

    // message
    void DispatchPacket(Packet* pPacket);
    void SendPacket(THREA_TYPE threadType, Packet* pPacket);

private:
    std::mutex _thread_lock;
    std::map<uint64, Thread*> _threads;
};

