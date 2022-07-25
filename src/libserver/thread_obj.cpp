#include "thread_obj.h"
#include "thread.h"

bool ThreadObject::IsActive() const {
    return _active;
}

void ThreadObject::Dispose() {
    _active = false;
	std::lock_guard<std::mutex> guardPacket(_packet_lock);
    _cachePackets.Dispose();
}

void ThreadObject::SetThread(Thread* pThread) {
    _pThread = pThread;
}

Thread* ThreadObject::GetThread() const {
    return _pThread;
}

void ThreadObject::SetObjType(int type) {
    _obj_type = type;
}

int ThreadObject::GetObjType() const {
    return _obj_type;
}

std::list<Packet*>* ThreadObject::GetPackets() {
    std::lock_guard<std::mutex> guard(_packet_lock);
    if (_cachePackets.CanSwap()) {
        _cachePackets.Swap();
    }
    return _cachePackets.GetReaderCache();
}

void ThreadObject::AddPacket(Packet* pPacket) {
    std::lock_guard<std::mutex> guard(_packet_lock);
    _cachePackets.GetWriterCache()->emplace_back(pPacket);
}
