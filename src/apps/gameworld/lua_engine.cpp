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

bool LuaEngine::loadLuaFile(char* filename) {
    int ret = luaL_loadfile(_luaState, filename);
    if(ret) {
        LOG_DEBUG("load lua failed." << "filename: " << filename);
        return false;
    }
    lua_pcall(_luaState, 0, 0, 0);
    return true;
}

bool LuaEngine::Init() {
    lua_pushcfunction(_luaState, GlobalLuaFunction::Send);
    lua_setglobal(_luaState, "send");

    char file1[] = "luascripts/init.lua";
    bool ret = loadLuaFile(file1);
    if(!ret) {
        return false;
    }
    char file2[] = "luascripts/gameworld/main.lua";
    ret = loadLuaFile(file2);
    if(!ret) {
        return false;
    }
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

int GlobalLuaFunction::Send(lua_State *l) {
    const char *s = luaL_checkstring(l, 1);
    LOG_DEBUG("lua call send. content: " << s);
    return 0;
}

