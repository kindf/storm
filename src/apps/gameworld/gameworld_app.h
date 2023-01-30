#pragma once
#include "server_app.h"
#include "network_listen.h"

class GameworldApp : public ServerApp {
public:
    explicit GameworldApp() : ServerApp(APP_TYPE::APP_GAMEWORLD){}
    void InitApp() override;
};

#include <string>
class ClientNetworkListen : public NetworkListen, public Singleton<ClientNetworkListen> {
};

