#include "pch.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "Job.h"
#include "Room.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"

enum
{
	WORKER_THICK = 64
};
void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		//방식은 프로젝트마다 다름 정답은 없다.
		LEndThickCount = ::GetTickCount64() + WORKER_THICK;

		//통신관련 처리 
		service->GetIocpCore()->Dispatch(10);

		//통신관련 일감 기다리다 없으면 빠져나와 JobQueue비우기

		//당장 실행해야할 일들 처리
		ThreadManager::DoGlobalQueueWork();

		//예약된 할 일들 처리
		ThreadManager::DistributeReserve();
	}
}
int main()
{
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\ProjectsV13;Database=ServerDB;Trusted_Connection=Yes;"));
	auto dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");

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


