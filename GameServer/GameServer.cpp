#include "pch.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "Protocol.pb.h"
#include "ServerPacketHandleler.h"


int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
	NetAddress(L"127.0.0.1", 7777),
	MakeShared<IocpCore>(),
	MakeShared<GameSession>,
	100
	);

	ASSERT_CRASH(service->Start());
	
	
	for(int32 i = 0;i<5;i++)
	{
		GThreadManager->Launch([=]()
		{
			while(true)
			{
				service->GetIocpCore()->Dispatch();
			}
		});
	}

	while(true)
	{
		Protocol::S_TEST pkt;
		pkt.set_id(1000);
		pkt.set_hp(100);
		pkt.set_attack(10);
		{
			auto buf = pkt.add_buffs();
			buf->set_buffid(100);
			buf->set_remaintime(19);
			buf->add_victims(4000);
		}
		{
			auto buf = pkt.add_buffs();
			buf->set_buffid(200);
			buf->set_remaintime(11);
			buf->add_victims(3000);
		}
		SendBufferRef sendBuffer = ServerPacketHandleler::MakeSendBffer(pkt);
		GSessionManager.Broadcast(sendBuffer);
		
		this_thread::sleep_for(550ms);

	}

	GThreadManager->Join();
}


