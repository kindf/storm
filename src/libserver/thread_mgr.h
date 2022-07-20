#pragma once

#include <mutex>
#include <map>
#include "common.h"
#include "thread.h"
#include "singleton.h"
#include "disposable.h"

class Packet;
class ThreadObject;

class ThreadMgr :public Singleton<ThreadMgr>, public IDisposable {
public:
    ThreadMgr();
    void StartAllWorkThread();

    bool IsStopAll();
    bool IsDisposeAll();
    void Dispose() override;    

    Thread* NewThread();
    bool AddObjWorkThread(ThreadObject* obj);

    bool NewObjThread(ThreadObject *obj);

    void RemoveObjByType(int objectType);
    // message
    /* void DispatchPacket(Packet* pPacket); */
    void SendPacket(THREAD_OBJECT_TYPE threadObjectType, Packet* pPacket);

    void Update();

private:
    uint64 _lastThreadSn{ 0 }; // 实现线程对象均分

    std::mutex _thread_lock;
    std::map<uint64, Thread*> _threads;

    std::mutex _work_thread_lock;
    std::map<uint64, Thread*> _work_threads;

    std::mutex _obj_lock;
    std::map<int, ThreadObject*> _objects;
};

