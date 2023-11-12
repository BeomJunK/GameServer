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
#pragma pack(1)
struct PKT_S_TEST
{
public:
    struct BuffListItem
    {
        uint64 buffId;
        float remainTime;

        uint16 victimsOffset;
        uint16 victimsCount;
        bool Validate(BYTE* packetStart, uint16 packetSize, OUT uint32& size)
        {
            if (victimsOffset + victimsCount * sizeof(uint64) > packetSize)
                return false;

            size += victimsCount * sizeof(uint64);
            return true;
        }
    };
    uint16 packetSize;
    uint16 packetId;
    uint64 id;
    uint32 hp;
    uint16 attack;
    uint16 buffsOffset;
    uint16 buffCount;

    bool IsValid()
    {
        uint32 size = 0;

        size += sizeof(PKT_S_TEST);

        if( packetSize < size)
            return false;

        if( buffsOffset + buffCount * packetSize > packetSize)
            return false;

        if( size != packetSize)
            return false;

        //가변데이터 크기 추가
        size += buffCount * sizeof(BuffListItem);

        auto buffList = GetBuffsList();
        for(int32 i = 0; i< buffList.Count(); ++i)
        {
            if(buffList[i].Validate((BYTE*)this, packetSize, OUT size) == false)
                return false;
        }
        
        return true;
    }
    using BuffsList = PacketList<PKT_S_TEST::BuffListItem>;
    using BuffsVictimsList = PacketList<uint64>;
    
    BuffsList GetBuffsList()
    {
        BYTE* data =    reinterpret_cast<BYTE*>(this);
        data += buffsOffset;
        return BuffsList(reinterpret_cast<PKT_S_TEST::BuffListItem*>(data), buffCount);
    }
    BuffsVictimsList GetBuffsVictimList(BuffListItem* buffsItem)
    {
        BYTE* data = reinterpret_cast<BYTE*>(this);
        data += buffsItem->victimsOffset;
        return BuffsVictimsList(reinterpret_cast<uint64*>(data), buffsItem->victimsCount);
    }
};
#pragma

void ClientPacketHandleler::Handle_CS_TEST(BYTE* buffer, int32 len)
{
    BufferReader br(buffer, len);
    PKT_S_TEST* pk = reinterpret_cast<PKT_S_TEST*>(buffer);
    if(pk->IsValid())
        return;
    
    PKT_S_TEST::BuffsList buffs=  pk->GetBuffsList();
    
    cout << "BufCount : " <<pk->buffCount << endl;
    for (auto& buff : buffs)
    {
        cout << "BufInfo : " << buff.buffId << " " << buff.remainTime << endl;

        PKT_S_TEST::BuffsVictimsList victims =  pk->GetBuffsVictimList(&buff);

        cout << "Victim Count : " << victims.Count() << endl;
        for (auto& victim : victims)
        {
            cout << "Victim : " << victim << endl;
        }

    }
}
