#include "pch.h"
#include "ServerPacketHandler.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
    // TODO : Log
    return false;
}
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
    if(pkt.success() == false)
        return true; //TODO: ���� ó��

    if(pkt.players().size() == 0)
    {
        //ĳ���� ���� â
    }
    cout << "����" << endl;
    //���� ����
    Protocol::C_ENTER_GAME enterPkt;
    enterPkt.set_playerindex(0); //ù��° ĳ������ ����
    auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterPkt);
    session->Send(sendBuffer);
    
	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
    if(pkt.success() == false)
        return false; //TODO: ���� ó��

    
    
    return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
    cout << pkt.msg() << endl;
    
    return true;
}

