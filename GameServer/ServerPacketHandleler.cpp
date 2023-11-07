#include "pch.h"
#include "ServerPacketHandleler.h"

#include "BufferReader.h"
#include "BufferWriter.h"

void ServerPacketHandleler::HandlePacket(BYTE* buffer, int32 len)
{
    BufferReader br(buffer, len);

    PacketHeader header;
    br.Peek(&header);

    switch (header.id)
    {
        default:
            {
                PacketHeader header = *((PacketHeader*)&buffer[0]);

                cout << "Pakcet ID : " << header.id << "Size : " << header.size << endl;      
            }
            break;
    }
}

SendBufferRef ServerPacketHandleler::Make_SC_TEST(uint64 id, uint32 hp, uint16 attack, Vector<BuffData> buffs)
{
    SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

    BufferWriter bw(sendBuffer->Buffer(), sendBuffer->GetAllocSize());

    PacketHeader* header = bw.Reserve<PacketHeader>();
		
    //id 체력 공격력을 넣는다
    bw << id << hp << attack;
    //가변데이터가 들어간다
    bw << (uint16)buffs.size();

    for(BuffData& data : buffs)
    {
        bw << data.buffId << data.remainTime;
    }

    
    header->size = bw.WriteSize();
    header->id = 1; // 1: 임시 Test Msg
    sendBuffer->Close(bw.WriteSize());

    return sendBuffer;
}
