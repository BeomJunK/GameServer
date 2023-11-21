#include "pch.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "Job.h"
#include "Room.h"


int main()
{
	{
		HealJob healJob;
		healJob._target = 1;
		healJob._healValue = 10;


		healJob.Execute();
	}
	ServerServiceRef service = MakeShared<ServerService>(
	NetAddress(L"127.0.0.1", 7777),
	MakeShared<IocpCore>(),
	MakeShared<GameSession>,
	100
	);
	ClientPacketHandler::Init();

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
		GRoom.FlushJob();
		this_thread::sleep_for(1ms);
	}
	GThreadManager->Join();

}


