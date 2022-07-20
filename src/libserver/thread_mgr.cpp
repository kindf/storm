#include "thread_mgr.h"
#include "common.h"
#include "log4_help.h"

ThreadMgr::ThreadMgr() {
}

void ThreadMgr::StartAllWorkThread() {
    auto iter = _threads.begin();
    while(iter != _threads.end()) {
        iter->second->Start();
        ++iter;
    }
}

Thread* ThreadMgr::NewThread() {
    auto pThread = new Thread();
    std::lock_guard<std::mutex> guard(_thread_lock);
    _threads.insert(std::make_pair(pThread->GetSN(), pThread));
    return pThread;
}

bool ThreadMgr::AddObjWorkThread(ThreadObject* obj) {
    {
        std::lock_guard<std::mutex> guard(_thread_lock);
        auto iter = _threads.begin();
        if(_lastThreadSn > 0) {
            iter = _threads.find(_lastThreadSn);
        }

        if(iter == _threads.end()) {
            LOG_WARN("[ThreadMgr::AddObjWorkThread] no thread.");
            return false;
        }

        do{
            ++iter;
            if(iter == _threads.end())
                iter = _threads.begin();
        }while(!(iter->second->IsRun()));

        auto pThread = iter->second;
        pThread->AddObject(obj);
        _lastThreadSn = pThread->GetSN();
    }
    std::lock_guard<std::mutex> obj_guard(_obj_lock);
    _objects.insert(std::make_pair(obj->GetObjType(), obj));
    return true;
}

bool ThreadMgr::NewObjThread(ThreadObject* obj) {
    auto pThread = NewThread();
    pThread->Start();
    pThread->AddObject(obj);
    std::lock_guard<std::mutex> guard(_obj_lock);
    _objects.insert(std::make_pair(obj->GetObjType(), obj));
    return true;
}

bool ThreadMgr::IsStopAll() {
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter) {
        if (!iter->second->IsStop()) {
            return false;
        }
    }
    return true;
}

bool ThreadMgr::IsDisposeAll() {
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter) {
        if (!iter->second->IsDispose()) {
            return false;
        }
    }
    return true;
}

void ThreadMgr::Dispose() {
    std::lock_guard<std::mutex> guard(_thread_lock);
    auto iter = _threads.begin();
    while (iter != _threads.end()) {
        Thread* pThread = iter->second;
        pThread->Dispose();
        delete pThread;
        ++iter;
    }
    _threads.clear();
}

void ThreadMgr::RemoveObjByType(int objectType) {
    std::lock_guard<std::mutex> guard(_obj_lock);
    _objects.erase(objectType);
}

/* void ThreadMgr::DispatchPacket(Packet* pPacket) { */
/*     std::lock_guard<std::mutex> guard(_thread_lock); */
/*     for (auto iter = _threads.begin(); iter != _threads.end(); ++iter) { */
/*         Thread* pThread = iter->second; */
/*         pThread->AddPacketToList(pPacket); */
/*     } */
/* } */

void ThreadMgr::SendPacket(THREAD_OBJECT_TYPE threadObjType, Packet* pPacket) {
    auto iter = _objects.find(threadObjType);
    if(iter == _objects.end()) {
        LOG_WARN("[ThreadMgr::SendPacket] can not find obj. obj type: " << threadObjType);
        return;
    }
    auto pThreadObj = iter->second;
    pThreadObj->AddPacket(pPacket);
    return;
}

void ThreadMgr::Update() {
    for(auto pThread : _work_threads) {
        pThread.second->Update();
    }
    
    for(auto pThread : _threads) {
        pThread.second->Update();
    }
}
