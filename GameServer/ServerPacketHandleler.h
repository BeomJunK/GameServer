#pragma once
#include "BufferWriter.h"

enum
{
    CS_TEST = 1
};
template <typename T>
class PacketList
{
public:
    PacketList() = default;
    PacketList(T* data, uint16 count) : _data(data), _count(count)
    {
    }

    T& operator[](uint16 index)
    {
        ASSERT_CRASH(index < _count);
        return _data[index];
    }

    uint16 Count() { return _count; }
    
private:
    T* _data;
    uint16 _count;
};

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

};
#pragma
class ServerPacketHandleler
{
public:
    static void HandlePacket(BYTE* buffer, int32 len);

};

class PKT_S_TEST_WRITE
{
public:
    using BuffsListItem = PKT_S_TEST::BuffListItem;
    using BuffsList = PacketList<PKT_S_TEST::BuffListItem>;
    using BuffsVictimsList = PacketList<uint64>;
    
    PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 attack)
    {
        _sendBuffer = GSendBufferManager->Open(4096);
        _bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->GetAllocSize());

        _pkt = _bw.Reserve<PKT_S_TEST>();
        _pkt->packetSize = 0; //to fill
        _pkt->packetId =  CS_TEST;
        _pkt->id = id;
        _pkt->hp = hp;
        _pkt->attack = attack;

        _pkt->buffsOffset = 0;//to fill
        _pkt->buffCount = 0;//to fill
    }
    BuffsList ReserveBuffsList(uint16 buffCount)
    {
        BuffsListItem* first = _bw.Reserve<BuffsListItem>(buffCount);
        _pkt->buffsOffset = (uint64)first - (uint64)_pkt;
        _pkt->buffCount = buffCount;
        return BuffsList(first,  buffCount);
    }
    BuffsVictimsList ReserveBuffsVitimsList(BuffsListItem* buffsItem, uint16 vitimsCount)
    {
        uint64* firstVitimsListItem = _bw.Reserve<uint64>(vitimsCount);
        buffsItem->victimsOffset = (uint64)firstVitimsListItem - (uint64)_pkt;
        buffsItem->victimsCount = vitimsCount;

        return BuffsVictimsList(firstVitimsListItem, vitimsCount);
    }
    SendBufferRef CloseAndReturn()
    {
        _pkt->packetSize = _bw.WriteSize();

        _sendBuffer->Close(_bw.WriteSize());
        return _sendBuffer;
    }
    
private:
    PKT_S_TEST* _pkt =nullptr;
    SendBufferRef _sendBuffer;
    BufferWriter _bw;
};
