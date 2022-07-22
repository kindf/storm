#pragma once
#include "thread_obj.h"
extern "C"
{
    #include "LuaJIT/src/lua.h"
    #include "LuaJIT/src/lualib.h"
    #include "LuaJIT/src/lauxlib.h"
};

class LuaEngine : public ThreadObject {
public:
    LuaEngine();
    ~LuaEngine();

    void Update();
    bool Init();
private:
    lua_State* _luaState;
};
