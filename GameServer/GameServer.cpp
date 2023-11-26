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
	//
	XmlNode root;
	XmlParser parser;
	//xml파일 읽어서 root노드 추출
	if (parser.ParseFromFile(L"GameDB.xml", OUT root) == false)
		return 0;

	//Table노드들 싹 긁어오기
	Vector<XmlNode> tables = root.FindChildren(L"Table");
	for (XmlNode& table : tables)
	{
		//테이블의 name , desc 추출
		String name = table.GetStringAttr(L"name");
		String desc = table.GetStringAttr(L"desc");

		//테이블안에있는  column 긁어오기
		Vector<XmlNode> columns = table.FindChildren(L"Column");
		for (XmlNode& column : columns)
		{
			String colName = column.GetStringAttr(L"name");
			String colType = column.GetStringAttr(L"type");
			bool nullable = !column.GetBoolAttr(L"notnull", false);
			String identity = column.GetStringAttr(L"identity");
			String colDefault = column.GetStringAttr(L"default");
			// Etc...
		}

		//Index글겅오기
		Vector<XmlNode> indices = table.FindChildren(L"Index");
		for (XmlNode& index : indices)
		{
			String indexType = index.GetStringAttr(L"type");
			bool primaryKey = index.FindChild(L"PrimaryKey").IsValid();
			bool uniqueConstraint = index.FindChild(L"UniqueKey").IsValid();

			Vector<XmlNode> columns = index.FindChildren(L"Column");
			for (XmlNode& column : columns)
			{
				String colName = column.GetStringAttr(L"name");
			}
		}
	}

	//위와 마찬가지로 프로시저도 추출
	Vector<XmlNode> procedures = root.FindChildren(L"Procedure");
	for (XmlNode& procedure : procedures)
	{
		String name = procedure.GetStringAttr(L"name");
		String body = procedure.FindChild(L"Body").GetStringValue();

		Vector<XmlNode> params = procedure.FindChildren(L"Param");
		for (XmlNode& param : params)
		{
			String paramName = param.GetStringAttr(L"name");
			String paramType = param.GetStringAttr(L"type");
			// TODO..
		}
	}
	//
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\ProjectsV13;Database=ServerDB;Trusted_Connection=Yes;"));
	
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


