#pragma once
#include "GameSessionManager.h"
#include "Protocol.pb.h"

class Player
{
public:
    uint64 playerId = 0;
    string name;
    Protocol::PlayerType type = Protocol::PLAYER_TYPE_NONE;
    GameSessionRef ownerSession; // TODO : 사이클 문제있음 나중에 끊어줘야 됨 
};

