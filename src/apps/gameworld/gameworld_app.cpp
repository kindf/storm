#include "gameworld_app.h"
#include "app_type_mgr.h"
#include "log4.h"
#include "thread_obj.h"
#include "common.h"
#include "thread_mgr.h"
#include "log4_help.h"
#include "console.h"
#include "lua_engine.h"
#include "network_listen.h"

#include "packet.h"
class TestThreadObj : public ThreadObject {
public:
    TestThreadObj():ThreadObject(TOT_TEST){}
    bool Init() {
        return true;
    }

    void Update() {
        /* LOG_DEBUG("TestThreadObj update"); */
    }

    void ProcessPacket(Packet*) {
        LOG_DEBUG("[TestThreadObj::ProcessPacket] Packet come.")
    }

    ~TestThreadObj(){
        LOG_DEBUG("[TestThreadObj::~TestThreadObj]")
    }
};

class TestConsoleCmd :public ConsoleCmd{
public:
    void RegisterHandler() override {
        OnRegisterHandler("-help", BindFunP1(this, &TestConsoleCmd::HandleHelp));
        OnRegisterHandler("-a", BindFunP1(this, &TestConsoleCmd::HandleTest));
   }
 
private:
    void HandleHelp(std::vector<std::string>& params) {
        LOG_DEBUG("[TestConsoleCmd::HandleHelp]");
    }

    void HandleTest(std::vector<std::string>& params) {
        if (!CheckParamCnt(params, 0))
            return;
        for(int i = 0; i < 1000; ++i) {
            Packet* pPacket = new Packet(0, 0);
            ThreadMgr::GetInstance()->SendPacket(TOT_TEST, pPacket);
        }
    }
 };

class TestConsole : public Console {
public:
    bool Init() {
        if(!Console::Init()) {
            LOG_ERROR("[TestConsole::Init] init failed.");
            return false;
        }
        Register<TestConsoleCmd>("test");
        return true;
    }
};

void GameworldApp::InitApp() {
    Log4::Instance(APP_TYPE::APP_GAMEWORLD);

    if(!ThreadMgr::GetInstance()->AddListenerToThread("127.0.0.1", 2233)) {
        LOG_ERROR("network listen error.");
    }

    TestThreadObj* pTest = new TestThreadObj();
    ThreadMgr::GetInstance()->AddObjWorkThread(TT_OTHER, pTest);
    
    Console* pConsole = new TestConsole();
    ThreadMgr::GetInstance()->AddObjWorkThread(TT_OTHER, pConsole);

    LuaEngine* pLuaEngine = new LuaEngine();
    ThreadMgr::GetInstance()->AddObjWorkThread(TT_LUAENGINE, pLuaEngine);
}
