#include "pch.h"
#include "ClientPacketHandleler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

void ClientPacketHandleler::HandlePacket(BYTE* buffer, int32 len)
{
    BufferReader br(buffer, len);

    PacketHeader header;
    br >> header;
    
    switch (header.id)
    {
    case S_TEST:
        Handle_CS_TEST(buffer, len);
        break;
    }
  
}

void ClientPacketHandleler::Handle_CS_TEST(BYTE* buffer, int32 len)
{
    Protocol::S_TEST pkt;

    ASSERT_CRASH(pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)));

    cout << pkt.id() << " " << pkt.hp() << " " << pkt.attack() << endl;

    cout << "BufSize : " << pkt.buffs_size();

    for(auto& buf : pkt.buffs())
    {
        cout << "BuffInfo: " << buf.buffid() << " " << buf.remaintime() << endl;
        cout << "Vitim Count: " << buf.victims_size() << endl;
        for(auto& vitim : buf.victims())
        {
            cout << "VitimInfo: " << vitim << endl;
        }
        
    }
    
}
