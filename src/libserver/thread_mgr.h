#pragma once

#include <mutex>

#include "common.h"
#include "thread.h"
#include "work_thread.h"
#include "singleton.h"
#include <map>

class Packet;
class ThreadObject;

class ThreadMgr :public Singleton<ThreadMgr> {
public:
    ThreadMgr();
    void StartAllWorkThread();

    bool IsStopAll();
    bool IsDisposeAll();
    void Dispose() override;    

    void NewThread();
    bool AddObjWorkThread(OBJECT_TYPE objectType, ThreadObject* obj);

    bool NewObjThread(OBJECT_TYPE objectType, ThreadObject *obj);

    // message
    /* void DispatchPacket(Packet* pPacket); */
    void SendPacket(OBJECT_TYPE objectType, Packet* pPacket);

private:
    uint64 _lastThreadSn{ 0 }; // 实现线程对象均分

    std::mutex _thread_lock;
    std::map<uint64, Thread*> _threads;

    std::mutex _work_thread_lock;
    std::map<uint64, Thread*> _work_threads;

    std::mutex _obj_lock;
    std::map<int, ThreadObject*> _objects;
};

