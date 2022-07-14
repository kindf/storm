#include "gameworld_app.h"
#include "app_type_mgr.h"
#include "log4.h"
#include "thread_obj.h"
#include "common.h"
#include "thread_mgr.h"
#include "log4_help.h"

class TestThreadObj : public ThreadObject, public Singleton<TestThreadObj> {
public:
    bool Init() {
        return true;
    }

    void Update() {
        LOG_DEBUG("TestThreadObj update");
    }
    ~TestThreadObj(){}
};

void GameworldApp::InitApp() 
{
    Log4::Instance(APP_TYPE::APP_GAMEWORLD);

    TestThreadObj* pTest = TestThreadObj::Instance();
    ThreadMgr::Instance()->AddObjWorkThread(TOT_TEST, pTest);
}
