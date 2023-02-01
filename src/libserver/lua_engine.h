#pragma once
#include "thread_obj.h"
#include <string>
extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
};

class LuaEngine : public ThreadObject {
public:
    LuaEngine(std::string path);
    ~LuaEngine();

    void Update();
    bool Init();
private:
    bool loadLuaFile(char* filename);
private:
    lua_State* _luaState;
    std::string _luaPath;
};

class LuaAPI {
public:
    static void Resiter(lua_State *l);
    static int Hello(lua_State *l);
    static int Send(lua_State *l);
}; 
