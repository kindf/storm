#pragma once
#include "server_app.h"

class GameworldApp : public ServerApp {
public:
    explicit GameworldApp() : ServerApp(APP_TYPE::APP_GAMEWORLD){}
    void InitApp() override;
};
