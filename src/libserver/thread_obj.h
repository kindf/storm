#pragma once

#include "sn_object.h"

enum THREAD_OBJECT_TYPE {
    TOT_DEFAULT = 0,
    TOT_CLIENT,
    TOT_WORLD,
    TOT_LOGIN,
    TOT_DATABASE,
    TOT_CONSOLE,
};

class Thread;
class ThreadObject : public SnObject {
public:

    virtual bool Init() = 0;
    virtual void Update() = 0;    

    void SetThread(Thread* pThread);
    Thread* GetThread() const;
    bool IsActive() const;
    void Dispose() override;
    bool CheckObjType(int type) const;
    int GetObjType() const {
        return _obj_type;
    }
    
protected:
    bool _active{ true };
    Thread* _pThread{ nullptr };
    int _obj_type{ TOT_DEFAULT };
};
