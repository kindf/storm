#pragma once

#include <mutex>
#include <vector>
#include <map>
#include <string>
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
    void Init();

    bool IsStopAll();
    bool IsDisposeAll();
    void Dispose() override;    

    bool AddObjWorkThread(THREAD_TYPE threadType, ThreadObject* obj);
    bool AddListenerToThread(std::string ip, int port);

    void RemoveObjByType(int objectType);
    // message
    /* void DispatchPacket(Packet* pPacket); */
    void SendPacket(THREAD_OBJECT_TYPE threadObjectType, Packet* pPacket);

private:
    std::mutex _thread_lock;
    std::vector<Thread*> _threads;

    std::mutex _obj_lock;
    std::map<int, ThreadObject*> _objects;
};

