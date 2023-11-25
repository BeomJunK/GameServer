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
	ASSERT_CRASH(GDBConnectionPool->Connect(1,
		L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\ProjectsV13;Database=ServerDB;Trusted_Connection=Yes;"));
	{
		auto query = L"									\
			DROP TABLE IF EXISTS [dbo].[Gold];			\
			CREATE TABLE [dbo].[Gold]					\
			(											\
				[id] INT NOT NULL PRIMARY KEY IDENTITY, \
				[gold] INT NULL,						\
				[name] NVARCHAR(50) NULL,				\
				[createDate] DATETIME NULL				\
			);";

				
		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}
	//Add Data
	for (int32 i = 0; i < 3; i++)
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		DBBind<3, 0> dbBind(*dbConn,L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?)");

		int32 gold = 100;
		dbBind.BindParam(0, gold);
		WCHAR name[100] = L"Kang";
		dbBind.BindParam(1, name);
		TIMESTAMP_STRUCT ts = {2023, 11, 26};
		dbBind.BindParam(2, ts);

		ASSERT_CRASH(dbBind.Validate());
		dbBind.Execute();
		
		GDBConnectionPool->Push(dbConn);
	}

	//Read
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		DBBind<1, 4> dbBind(*dbConn,L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (?)");

		int32 gold = 100;
		dbBind.BindParam(0, gold);

		int32 outId = 0;
		int32 outGold = 0;
		WCHAR outName[100];
		TIMESTAMP_STRUCT outDate = {};
		dbBind.BindCol(0, OUT outId);
		dbBind.BindCol(1, OUT outGold);
		dbBind.BindCol(2, OUT outName);
		dbBind.BindCol(3, OUT outDate);

		ASSERT_CRASH(dbBind.Validate());
		
		dbBind.Execute();

		while (dbConn->Fetch())
		{
			wcout << "Id: " << outId << " Gold : " << outGold << " Name: " << outName << endl;
			wcout << "Date : " << outDate.year << "/" << outDate.month << "/" << outDate.day << endl;
		}
		GDBConnectionPool->Push(dbConn);
	}
	return 0;


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


