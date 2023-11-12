#include "pch.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
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
		PKT_S_TEST_WRITE pkWriter(1001, 100, 10);

		PKT_S_TEST_WRITE::BuffsList buffList = pkWriter.ReserveBuffsList(3);
		buffList[0] = {100, 1.5f};
		buffList[1] = {200, 2.5f};
		buffList[2] = {300, 3.5f};

		PKT_S_TEST_WRITE::BuffsVictimsList vic0 = pkWriter.ReserveBuffsVitimsList(&buffList[0], 3);
		{
			 vic0[0] = 111;
			 vic0[1] = 111;
			 vic0[2] = 111;
		}

		PKT_S_TEST_WRITE::BuffsVictimsList vic1 = pkWriter.ReserveBuffsVitimsList(&buffList[1], 3);
		{
			vic1[0] = 222;
			vic1[1] = 222;
			vic1[2] = 222;
		}

		PKT_S_TEST_WRITE::BuffsVictimsList vic2 = pkWriter.ReserveBuffsVitimsList(&buffList[2], 3);
		{
			vic2[0] = 333;
			vic2[1] = 333;
			vic2[2] = 333;
		}
		SendBufferRef sendBuffer = pkWriter.CloseAndReturn();
		
		this_thread::sleep_for(550ms);
		GSessionManager.Broadcast(sendBuffer);
	}

	GThreadManager->Join();
}


