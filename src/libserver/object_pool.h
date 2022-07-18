#pragma once
#include <queue>
#include <list>
#include "sn_object.h"
#include "object_block.h"
#include "packet.h"
#include "object_pool_interface.h"
#include "thread_obj.h"
#include "cache_refresh.h"
#include "object_pool_mgr.h"
#include "log4_help.h"

template <typename T>
class DynamicObjectPool :public IDynamicObjectPool {
public:
    static DynamicObjectPool<T>* GetInstance()
    {
        std::lock_guard<std::mutex> guard(_instanceLock);

        if (_pInstance == nullptr) {
            _pInstance = new DynamicObjectPool<T>();
            DynamicObjectPoolMgr::GetInstance()->AddPool(_pInstance);
        }

        return _pInstance;
    }

    void DestroyInstance() override {
        std::lock_guard<std::mutex> guard(_instanceLock);
        if (_pInstance == nullptr)
            return;

        delete _pInstance;
        _pInstance = nullptr;
    }

    DynamicObjectPool();
    ~DynamicObjectPool();

    template<typename ...Targs>
    T* MallocObject(Targs... args);

    void Update() override;
    void FreeObject(ObjectBlock* pObj) override;

    void Show();

private:
    void CreateOne();

private:
    std::queue<T*> _free;
    std::mutex _freeLock;

    std::mutex _inUseLock;
    CacheRefresh<T> _objInUse;

    static DynamicObjectPool<T>* _pInstance;
    static std::mutex _instanceLock;

#if _DEBUG
    int _totalCall{ 0 };
#endif
};

template <typename T>
DynamicObjectPool<T>* DynamicObjectPool<T>::_pInstance = nullptr;

template <typename T>
std::mutex DynamicObjectPool<T>::_instanceLock;


template <typename T>
DynamicObjectPool<T>::DynamicObjectPool() {
}

template <typename T>
void DynamicObjectPool<T>::CreateOne() {
    T* pObj = new T(this);
    _free.push(pObj);
}

template <typename T>
DynamicObjectPool<T>::~DynamicObjectPool() {
    // ������֮ǰ������һ��Update����inuse�еĶ���ص�Free��
    Update();

#if _DEBUG
    if (_objInUse.GetReaderCache()->size() > 0)
    {
        // ����û��ȫ����������һ��������~
        LOG_DEBUG("dispose object pool error. type:" << typeid(T).name() << " inUse size:" << _objInUse.GetReaderCache()->size() << " free size:" << _free.size());
    }
#endif

    LOG_DEBUG("dispose object pool. type:" << typeid(T).name());

    while (_free.size() > 0) {
        auto iter = _free.front();
        delete iter;
        _free.pop();
    }

    _objInUse.Dispose();
}

template <typename T>
template <typename ... Targs>
T* DynamicObjectPool<T>::MallocObject(Targs... args) {
    _freeLock.lock();
    if (_free.size() == 0) {
        CreateOne();
    }

#if _DEBUG
    _totalCall++;
#endif

    auto pObj = _free.front();
    _free.pop();
    _freeLock.unlock();

    pObj->ResetSN();
    pObj->TakeoutFromPool(std::forward<Targs>(args)...);

    _inUseLock.lock();
    _objInUse.GetAddCache()->push_back(pObj);
    _inUseLock.unlock();

    return pObj;
}

template <typename T>
void DynamicObjectPool<T>::Update() {
    std::list<T*> freeObjs;
    _inUseLock.lock();
    if (_objInUse.CanSwap()) {
        freeObjs = _objInUse.Swap();
    }
    _inUseLock.unlock();

    std::lock_guard<std::mutex> guard(_freeLock);
    for (auto one : freeObjs) {
        _free.push(one);
    }
}

template<typename T>
inline void DynamicObjectPool<T>::FreeObject(ObjectBlock* pObj) {
    std::lock_guard<std::mutex> guard(_inUseLock);
    _objInUse.GetRemoveCache()->emplace_back(dynamic_cast<T*>(pObj));
}

template <typename T>
void DynamicObjectPool<T>::Show() {
    std::lock_guard<std::mutex> guard(_freeLock);
    std::lock_guard<std::mutex> guardInUse(_inUseLock);
    auto count = _objInUse.GetReaderCache()->size() + _objInUse.GetAddCache()->size() + _objInUse.GetRemoveCache()->size();
    LOG_INFO("*************************** \n pool total count:\t" << _free.size() + count << "\n" << "free count:\t\t" << _free.size() << "\n" << "in use count:\t" << count << "\n");
#if _DEBUG
    LOG_DEBUG("total call:\t" << _totalCall << "\n");
#endif
}
