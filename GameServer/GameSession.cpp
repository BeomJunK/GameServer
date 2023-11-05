#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void GameSession::OnConnected()
{
    cout << "클라 접속" << endl;
    GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

int32 GameSession::OnRecvPacket(BYTE* buffer, int32 len) 
{
    PacketHeader header = *((PacketHeader*)&buffer[0]);

    cout << "Pakcet ID : " << header.id << "Size : " << header.size << endl;
    
   
    return len;
}
void GameSession::OnSend(DWORD len)
{   
}

void GameSession::OnDisconnected()
{
    cout << "클라 끊음" << endl;
}
