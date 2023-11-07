#pragma once
#include "Session.h"

class GameSession : public PacketSession
{
public:
    ~GameSession()
    {
        cout << "~GameSession!" << endl;
    }
    void OnConnected() override;
    void OnRecvPacket(BYTE* buffer, int32 len) override;
    void OnSend(DWORD len) override;
    void OnDisconnected() override;
};