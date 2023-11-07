#include "pch.h"
#include "ClientPacketHandleler.h"
#include "BufferReader.h"


struct BuffData
{
    uint64 id;
    float remainTime;
};

void ClientPacketHandleler::HandlePacket(BYTE* buffer, int32 len)
{
    BufferReader br(buffer, len);

    PacketHeader header;
    br >> header;
    
    switch (header.id)
    {
    case CS_TEST:
        Handle_CS_TEST(buffer, len);
        break;
    }
  
}

void ClientPacketHandleler::Handle_CS_TEST(BYTE* buffer, int32 len)
{
    BufferReader br(buffer, len);
    PacketHeader header;
    br >> header;

    uint64 id;
    uint32 hp;
    uint16 attack;
    br >> id >> hp >> attack;
    cout << "Pakcet ID : " << id << "hp : " << hp << "atk : " << attack <<endl;

    Vector<BuffData> buffs;
    uint16 buffSize;
    br >> buffSize;
    buffs.resize(buffSize);

    for (int i = 0; i < buffSize; ++i)
    {
        br >> buffs[i].id >> buffs[i].remainTime;
    }

    cout << "BufCount : " <<buffSize << endl;
    for (int i = 0; i < buffSize; ++i)
    {
        cout << "BufId : " <<buffs[i].id << "BufremainTime : " <<buffs[i].remainTime  << endl;
    }
}
