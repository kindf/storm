#pragma once
#include <memory>
#include "network.h"
#include "util_time.h"

class ConnectObj;
class Packet;

class NetworkConnector : public Network {
public:
    bool Init() override;
    virtual bool Connect(std::string ip, int port);
    void Update() override;
    bool IsConnected() const;

private:
    void TryCreateConnectObj();

protected:
    std::string _ip{ "" };
    int _port{ 0 };
    timeutil::Time _nextConnectTime{ 0 };
};

