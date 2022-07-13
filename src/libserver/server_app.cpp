#include "common.h"
#include "server_app.h"
/* #include "network_listen.h" */
/* #include "object_pool_mgr.h" */
/* #include "yaml.h" */
#include "res_path.h"
/* #include "app_type_mgr.h" */
#include "log4_help.h"

ServerApp::ServerApp(APP_TYPE  appType) {
    signal(SIGINT, Signalhandler);

    _appType = appType;

    DynamicObjectPoolMgr::Instance();
    Global::Instance();
    ThreadMgr::Instance();
    AppTypeMgr::Instance();
    ResPath::Instance();
    Yaml::Instance();
    _pThreadMgr = ThreadMgr::GetInstance();
    UpdateTime();

    // �����߳�
    for (int i = 0; i < 2; i++) {
        _pThreadMgr->NewThread();
    }

    _pThreadMgr->StartAllThread();
}

ServerApp::~ServerApp() {
    _pThreadMgr->DestroyInstance();
}

void ServerApp::Signalhandler(const int signalValue) {
    switch (signalValue)
    {
    case SIGSTOP:
    case SIGQUIT:
    case SIGTERM:
    case SIGINT:
        Global::GetInstance()->IsStop = true;
        break;
    }

    LOG_DEBUG("\nrecv signal. value:" << signalValue << " Global IsStop::" << Global::GetInstance()->IsStop);
}

void ServerApp::Dispose() {
    _pThreadMgr->Dispose();
}

void ServerApp::Run() const {
    while (!Global::GetInstance()->IsStop) {
        UpdateTime();
        _pThreadMgr->Update();
        DynamicObjectPoolMgr::GetInstance()->Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // ֹͣ�����߳�
    LOG_DEBUG("stoping all threads...");
    bool isStop;
    do {
        isStop = _pThreadMgr->IsStopAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } while (!isStop);

    // �ͷ������߳���Դ
    LOG_DEBUG("disposing all threads...");

    // 1.���߳���Դ
    bool isDispose;
    do {
        isDispose = _pThreadMgr->IsDisposeAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } while (!isDispose);

    // 2.���߳���Դ
    _pThreadMgr->Dispose();

    LOG_DEBUG("disposing all pool...")
    DynamicObjectPoolMgr::GetInstance()->Update();
    DynamicObjectPoolMgr::GetInstance()->Dispose();
    DynamicObjectPoolMgr::DestroyInstance();

    Global::DestroyInstance();
    ThreadMgr::DestroyInstance();
}


void ServerApp::UpdateTime() const {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    Global::GetInstance()->TimeTick = tv.tv_sec * 1000 +  tv.tv_usec * 0.001;
}

bool ServerApp::AddListenerToThread(std::string ip, int port) const {
    NetworkListen* pListener = new NetworkListen();
    if (!pListener->Listen(ip, port)) {
        delete pListener;
        return false;
    }

    _pThreadMgr->AddNetworkToThread(APP_Listen, pListener);
    return true;
}