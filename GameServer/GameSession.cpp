#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"

void GameSession::OnConnected()
{
    wcout.imbue(locale("kor"));
    wstring str = L"Ŭ�� ����";
    wcout << str << endl;
    GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len) 
{
    PacketSessionRef session = GetPacketSessionRef();

    //TODO : PacketIdüũ �ʿ�
    //PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

    ClientPacketHandler::HandlePacket(session, buffer, len);
}
void GameSession::OnSend(DWORD len)
{   
}

void GameSession::OnDisconnected()
{
    wcout << L"Ŭ�� ����" << endl;
    GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}
