#include "pch.h"
#include "Service.h"
#include "Session.h"

class GameSession : public Session
{
public:
	~GameSession()
	{
		cout << "~GameSession!" << endl;
	}
	void OnConnected() override
	{
		cout << "클라 접속" << endl;
	}

	int32 OnRecv(BYTE* buffer, int32 len) override
	{
		cout << "데이터 받음 : " << len << endl;
		Send(buffer, len);
		return len;
	}
	void OnSend(DWORD len)
	{
		cout << "데이터 보냄 : " << len << endl;
	}
	
};
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


	GThreadManager->Join();
}


