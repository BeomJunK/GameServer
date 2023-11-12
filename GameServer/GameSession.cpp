#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandleler.h"

void GameSession::OnConnected()
{
    wcout.imbue(locale("kor"));
    wstring str = L"클라 접속";
    wcout << str << endl;
    GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len) 
{
    ServerPacketHandleler::HandlePacket(buffer, len);
}
void GameSession::OnSend(DWORD len)
{   
}

void GameSession::OnDisconnected()
{
    wcout << L"클라 끊음" << endl;
    GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}
