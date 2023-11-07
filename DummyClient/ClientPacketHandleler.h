#pragma once
enum
{
    CS_TEST = 1
};
class ClientPacketHandleler
{
public:
    static void HandlePacket(BYTE* buffer, int32 len);
    static void Handle_CS_TEST(BYTE* buffer, int32 len);
};

