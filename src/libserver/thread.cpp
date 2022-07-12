#include "thread.h"
#include "global.h"

#include <iterator>

void Thread::Update() {
    _packet_lock.lock();
    if (_cachePackets.CanSwap()) {
        _cachePackets.Swap();
    }
    _packet_lock.unlock();

    auto pMsgList = _cachePackets.GetReaderCache();
    for (auto itMsg = pMsgList->begin(); itMsg != pMsgList->end(); ++itMsg) {
        _threadObj->ProcessPacket(*itMsg);
    }

    _threadObj->Update();

    // ·Ç¼¤»î×´Ì¬£¬É¾³ý
    if (!_threadObj->IsActive()) {
    }
    pMsgList->clear();
}

void ThreadObjectList::AddPacketToList(Packet* pPacket) {
    std::lock_guard<std::mutex> guard(_packet_lock);
    _cachePackets.GetWriterCache()->emplace_back(pPacket);
}

void ThreadObjectList::Dispose() {
	std::lock_guard<std::mutex> guardObj(_obj_lock);
    _objlist.Dispose();

	std::lock_guard<std::mutex> guardPacket(_packet_lock);
    _cachePackets.Dispose();
}

Thread::Thread(ThreadObject* pThreadObj): _threadObj(pThreadObj) {
    _state = ThreadState_Init;
}

void Thread::Start() {
    _thread = std::thread([this]() {
            _state = ThreadState_Run;
            while (!Global::GetInstance()->IsStop)
            {
            Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
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

