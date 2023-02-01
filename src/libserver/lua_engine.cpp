#include "lua_engine.h"
#include "log4_help.h"

LuaEngine::LuaEngine(std::string path): _luaPath(path) {
    _luaState = luaL_newstate();
    luaopen_base(_luaState);
    luaL_openlibs(_luaState);
    LuaAPI::Resiter(_luaState);
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
    char file1[] = "luascripts/init.lua";
    bool ret = loadLuaFile(file1);
    if(!ret) {
        return false;
    }
    char file2[32];
    sprintf(file2, "luascripts/%s/main.lua", _luaPath.c_str());
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

void LuaAPI::Resiter(lua_State *l) {
    static luaL_Reg lua_libs[] = {
        {"send", Send},
        {"hello", Hello},
        {NULL, NULL}
    };
    luaL_newlib(l, lua_libs);
    lua_setglobal(l, "storm");
}

int LuaAPI::Hello(lua_State *l) {
    LOG_DEBUG("Hello World!!!!");
    return 0;
}

int LuaAPI::Send(lua_State *l) {
    const char *s = luaL_checkstring(l, 1);
    LOG_DEBUG("lua call send. content: " << s);
    return 0;
}

