#pragma once
#include "BufferWriter.h"
#include "Protocol.pb.h"

enum
{
    S_TEST = 1
};
class ServerPacketHandleler
{
public:
    static void HandlePacket(BYTE* buffer, int32 len);

    static SendBufferRef MakeSendBffer(Protocol::S_TEST& pkt);
};

template <typename T>
SendBufferRef _MakeSendBuffer(T& pkt, uint16 packetID)
{
    const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
    const uint16 packetSize = dataSize + sizeof(PacketHeader);

    SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);

    PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
    header->id = packetID;
    header->size = packetSize;

    ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
    
    sendBuffer->Close(packetSize);
    return sendBuffer;
    
}