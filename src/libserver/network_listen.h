#pragma once

#include "network.h"

class NetworkListen :public Network {
public:
    bool Init() override;
    bool Listen(std::string ip, int port);
    void Update() override;
    void ProcessPacket(Packet* pPacket);

protected:
    virtual int Accept();
};
