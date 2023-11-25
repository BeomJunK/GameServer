#include "pch.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "Job.h"
#include "Room.h"

enum
{
	WORKER_THICK = 64
};
void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndThickCount = ::GetTickCount64() + WORKER_THICK;

		//일감 기다리다 없으면 빠져나와 JobQueue비우기
		//프로젝트마다 다름
		service->GetIocpCore()->Dispatch(10);

		ThreadManager::DoGlobalQueueWork();
	}
}
int main()
{
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
		GThreadManager->Launch([&service]()
		{
			while(true)
			{
				DoWorkerJob(service);
			}
		});
	}

	DoWorkerJob(service);

	GThreadManager->Join();

}


