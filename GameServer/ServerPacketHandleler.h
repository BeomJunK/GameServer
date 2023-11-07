#pragma once
enum
{
    CS_TEST = 1
};

struct BuffData
{
    uint64 buffId;
    float remainTime;
};
class ServerPacketHandleler
{
public:
    static void HandlePacket(BYTE* buffer, int32 len);

    static SendBufferRef Make_SC_TEST(uint64 id, uint32 hp, uint16 attack, Vector<BuffData> buffs);
};

