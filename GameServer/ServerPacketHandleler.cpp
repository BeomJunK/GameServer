#include "pch.h"
#include "ServerPacketHandleler.h"
#include "BufferReader.h"

void ServerPacketHandleler::HandlePacket(BYTE* buffer, int32 len)
{
    BufferReader br(buffer, len);

    PacketHeader header;
    br.Peek(&header);

    switch (header.id)
    {
    case 1:

        break;
        default:
            {
                PacketHeader header = *((PacketHeader*)&buffer[0]);

                cout << "Pakcet ID : " << header.id << "Size : " << header.size << endl;      
            }
            break;
    }
}

SendBufferRef ServerPacketHandleler::MakeSendBffer(Protocol::S_TEST& pkt)
{
    return _MakeSendBuffer(pkt, S_TEST);
}
