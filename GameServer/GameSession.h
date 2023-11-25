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

public:
    Vector<PlayerRef> _players; //접속중인 유저목록(?)

    PlayerRef _currentPlayer; //내가 접속중인 플레이어
    weak_ptr<class Room> _room; //어떤룸인지 모르니 들고잇기

};