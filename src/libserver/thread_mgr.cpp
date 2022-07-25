#include "thread_mgr.h"
#include "common.h"
#include "log4_help.h"

ThreadMgr::ThreadMgr() {
}

void ThreadMgr::Init() {
    std::lock_guard<std::mutex> guard(_thread_lock);
    for(int i = 0; i < TT_COUNT; ++i) {
        auto pThread = new Thread();
        _threads.push_back(pThread);
    }
}

void ThreadMgr::StartAllWorkThread() {
    for(auto iter = _threads.begin(); iter != _threads.end(); ++iter) {
        auto pThread = *iter;
        pThread->Start();
    }
}

bool ThreadMgr::AddObjWorkThread(THREAD_TYPE threadType, ThreadObject* obj) {
        if(threadType < 0 || threadType >= TT_COUNT) {
            LOG_WARN("[ThreadMgr::AddObjWorkThread] invaild thread type:" << threadType);
            return false;
        }
    {
        std::lock_guard<std::mutex> guard(_thread_lock);
        auto pThread = _threads.at(threadType);
        pThread->AddObject(obj);
    }
    std::lock_guard<std::mutex> obj_guard(_obj_lock);
    _objects.insert(std::make_pair(obj->GetObjType(), obj));
    return true;
}

bool ThreadMgr::IsStopAll() {
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter) {
        auto pThread = *iter;
        if (!pThread->IsStop()) {
            return false;
        }
    }
    return true;
}

bool ThreadMgr::IsDisposeAll() {
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter) {
        auto pThread = *iter;
        if (!pThread->IsDispose()) {
            return false;
        }
    }
    return true;
}

void ThreadMgr::Dispose() {
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter) {
        auto pThread = *iter;
        delete pThread;
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
