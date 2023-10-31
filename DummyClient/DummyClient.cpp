#include "pch.h"
#include <iostream>
#include "Session.h"
#include "Service.h"


void HandleError(const char* cause)
{
	//에러 몇번으로 실패했는지 받을 수 있다.
	int32 errCode = ::WSAGetLastError();
	cout << cause << errCode << endl;
}

char sendBuffer[] = "Hello World";
class ServerSession : public Session
{
public:
	~ServerSession()
	{
		cout << "~ServerSession!" << endl;
	}
	void OnDisconnected() override
	{
		cout << "OnDisconnected!" << endl;
	}
	void OnConnected() override
	{
		cout << "서버접속!" << endl;
		Send((BYTE*)sendBuffer, sizeof(sendBuffer));
	}
	void OnSend(DWORD len) override
	{
		cout << "데이터  보냄" << len << endl;
	}
	
	int32 OnRecv(BYTE* buffer, int32 len) override
	{
		cout << "데이터 받음" << len << endl;
		//this_thread::sleep_for(1s);
		Send((BYTE*)sendBuffer, sizeof(sendBuffer));
		return len;
	}
};
int main()
{
	this_thread::sleep_for(2s);
	ClientServiceRef service = MakeShared<ClientService>(
	NetAddress(L"127.0.0.1", 7777),
	MakeShared<IocpCore>(),
	MakeShared<ServerSession>,
	1
	);

	ASSERT_CRASH(service->Start());
	
	for(int32 i = 0;i<2;i++)
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