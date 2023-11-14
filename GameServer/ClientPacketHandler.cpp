#include "pch.h"
#include "ClientPacketHandler.h"

PacketHandleFunc GPacketHandler[UINT16_MAX];
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_C_TEST(PacketSessionRef& session, Protocol::C_TEST& pkt)
{
	return false;
}




