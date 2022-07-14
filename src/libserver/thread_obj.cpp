#include "thread_obj.h"
#include "thread.h"

bool ThreadObject::IsActive() const {
    return _active;
}

void ThreadObject::Dispose() {
    _active = false;
}

void ThreadObject::SetThread(Thread* pThread) {
    _pThread = pThread;
}

Thread* ThreadObject::GetThread() const {
    return _pThread;
}

bool ThreadObject::CheckObjType(int type) const {
    return _obj_type == type;
}

int ThreadObject::GetObjType() const {
    return _obj_type;
}
