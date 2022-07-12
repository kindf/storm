#include "thread_mgr.h"
#include "common.h"
#include <iostream>

#include "network.h"
#include "network_listen.h"

ThreadMgr::ThreadMgr() {
}

bool ThreadMgr::AddObjToThread(THREA_TYPE threadType, ThreadObject* obj){
    std::lock_guard<std::mutex> guard(_thread_lock);
    auto pThread = new Thread(obj);
    pThread->Start();
    _threads.insert(std::make_pair(pThread->GetSN(), pThread));
    return true;
}

bool ThreadMgr::IsStopAll()
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter) 
    {
        if (!iter->second->IsStop())
        {
            return false;
        }
    }
    return true;
}

bool ThreadMgr::IsDisposeAll()
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        if (!iter->second->IsDispose())
        {
            return false;
        }
    }
    return true;
}

void ThreadMgr::Dispose()
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    auto iter = _threads.begin();
    while (iter != _threads.end())
    {
        Thread* pThread = iter->second;
        pThread->Dispose();
        delete pThread;
        ++iter;
    }
    _threads.clear();
}

void ThreadMgr::DispatchPacket(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        Thread* pThread = iter->second;
        pThread->AddPacketToList(pPacket);
    }
}

void ThreadMgr::SendPacket(THREA_TYPE threadType, Packet* pPacket) {
    auto iter = _threads.find(threadType);
    if(iter == _threads.end()) {
        std::cout << "[ThreadMgr::SendPacket] can not find thread. thread type: " << threadType << std::endl;
        return;
    }
    auto pThread = iter->second;
    pThread->AddPacketToList(pPacket);
    return;
}
