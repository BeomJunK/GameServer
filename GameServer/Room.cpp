#include "pch.h"
#include "Room.h"
#include "GameSession.h"
#include "Player.h"
#include "Job.h"

shared_ptr<Room> GRoom = make_shared<Room>();

void Room::Enter(PlayerRef player)
{
    WRITE_LOCK
    _players[player->playerId] = player;
}

void Room::Leave(PlayerRef player)
{
    WRITE_LOCK
    _players.erase(player->playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
    WRITE_LOCK;
    for(auto& p : _players)
    {
        p.second->ownerSession->Send(sendBuffer);
    }
}
