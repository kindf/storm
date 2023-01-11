#include "lua_engine.h"
#include "log4_help.h"

LuaEngine::LuaEngine() {
    _luaState = luaL_newstate();
    luaopen_base(_luaState);
    luaL_openlibs(_luaState);
}

LuaEngine::~LuaEngine() {
    lua_close(_luaState);
}

bool LuaEngine::Init() {
    int ret = luaL_loadfile(_luaState, "luascripts/gameworld/main.lua");
    if(ret) {
        LOG_DEBUG("load lua failed.");
        return false;
    }
    lua_pcall(_luaState, 0, 0, 0);
    return true;
}

void LuaEngine::Update() {
    lua_getglobal(_luaState, "Update");
    int ret = lua_pcall(_luaState, 0, 0, 0);
    if(ret) {
        const char* pErrorMsg = lua_tostring(_luaState, -1);
        LOG_DEBUG("call lua error. msg:" << pErrorMsg);
    }
}
