#include <iterator>
#include "thread.h"
#include "global.h"
#include "thread_mgr.h"

void Thread::Update() {

    _objs_lock.lock();
    if(_objList.CanSwap()) {
        auto pDelList = _objList.Swap();
        for(auto pOne : pDelList) {
            ThreadMgr::GetInstance()->RemoveObjByType(pOne->GetObjType());
            pOne->Dispose();
            delete pOne;
        }
    }


    _packet_lock.lock();
    if (_cachePackets.CanSwap()) {
        _cachePackets.Swap();
    }
    _packet_lock.unlock();

    auto pList = _objList.GetReaderCache();
    auto pMsgList = _cachePackets.GetReaderCache();
    for(auto iter = pList->begin(); iter != pList->end(); ++iter) {
        auto pObj = (*iter);
        for (auto itMsg = pMsgList->begin(); itMsg != pMsgList->end(); ++itMsg) {
            auto pPacket = (*itMsg);
            if(pObj->CheckObjType(pPacket->GetDest())) {
                pObj->ProcessPacket(*itMsg);
            }
        }
        pObj->Update();

        // 非激活状态，删除
        if (!pObj->IsActive()) {
            _objList.GetRemoveCache()->emplace_back(pObj);
        }
    }


    pMsgList->clear();
}

void Thread::AddPacketToList(Packet* pPacket) {
    std::lock_guard<std::mutex> guard(_packet_lock);
    _cachePackets.GetWriterCache()->emplace_back(pPacket);
}

void Thread::AddObject(ThreadObject* pThreadObj) {
    std::lock_guard<std::mutex> guard(_objs_lock);

    // 在加入之前初始化一下
    if (!pThreadObj->Init()) {
        std::cout << "AddObject Failed. ThreadObject init failed." << std::endl;
    }else {
        /* pThreadObj->RegisterMsgFunction(); */
        _objList.GetAddCache()->emplace_back(pThreadObj);
        const auto pThread = dynamic_cast<Thread*>(this);
        if (pThread != nullptr)
            pThreadObj->SetThread(pThread);
    }
}

void Thread::Dispose() {
	std::lock_guard<std::mutex> guardObj(_objs_lock);
    _objList.Dispose();

	std::lock_guard<std::mutex> guardPacket(_packet_lock);
    _cachePackets.Dispose();
}

Thread::Thread() {
    _state = ThreadState_Init;
}

void Thread::Start() {
    _thread = std::thread([this]() {
            _state = ThreadState_Run;
            while (!Global::GetInstance()->IsStop)
            {
                Update();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::cout << "thread running" << std::endl;
            }

            const auto theadId = _thread.get_id();
            std::cout << "close thread [1/2]. thread sn:" << this->GetSN() << " thread id:" << theadId << std::endl;
            _state = ThreadState_Stoped;
            });
}

bool Thread::IsRun() const {
    return _state == ThreadState_Run;
}

bool Thread::IsStop() const {
    return _state == ThreadState_Stoped;
}

bool Thread::IsDispose() {
    if (_thread.joinable()) {
        const auto theadId = _thread.get_id();
        _thread.join();
        std::cout << "close thread [2/2]. thread sn:" << this->GetSN() << " thread id:" << theadId << std::endl;
        return true;
    }

    return false;
}

