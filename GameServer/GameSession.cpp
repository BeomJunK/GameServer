#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void GameSession::OnConnected()
{
    cout << "클라 접속" << endl;
    GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

int32 GameSession::OnRecv(BYTE* buffer, int32 len) 
{
    cout << "데이터 받음 : " << len << endl;

    SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
    ::memcpy(sendBuffer->Buffer(), buffer, len);
    sendBuffer->Close(len);

    GSessionManager.Broadcast(sendBuffer);
    return len;
}
void GameSession::OnSend(DWORD len)
{
    cout << "데이터 보냄 : " << len << endl;
}

void GameSession::OnDisconnected()
{
    cout << "클라 끊음" << endl;
}
