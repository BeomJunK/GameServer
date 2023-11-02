#pragma once
#include "Session.h"

class GameSession : public Session
{
public:
    ~GameSession()
    {
        cout << "~GameSession!" << endl;
    }
    void OnConnected() override;
    int32 OnRecv(BYTE* buffer, int32 len) override;
    void OnSend(DWORD len) override;
    void OnDisconnected() override;
};