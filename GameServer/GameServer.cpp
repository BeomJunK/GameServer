#include "pch.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "Job.h"
#include "Room.h"

void HealByValue(int64 target, int32 value, int32 val2)
{
	cout << target << "한테 힐: " << value <<" 대미지 :" << val2 << endl;
}
class Knight
{
public:
	void HealMe(int32 val)
	{
		cout << "힐" << val << endl;
	}
};

int main()
{
	{
		FuncJob<void, int64, int32, int32> job(HealByValue,100, 10, -2);
		job.Execute();
	}
	{
		Knight k;
		MemberJob< Knight, void, int32> memJob(&k, &Knight::HealMe, 10);
		memJob.Execute();
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


