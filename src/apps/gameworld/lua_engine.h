#pragma once
#include "thread_obj.h"
extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
};

class LuaEngine : public ThreadObject {
public:
    LuaEngine();
    ~LuaEngine();

    void Update();
    bool Init();
private:
    bool loadLuaFile(char* filename);
private:
    lua_State* _luaState;
};

class GlobalLuaFunction {
public:
    static int Send(lua_State *l);
}; 
