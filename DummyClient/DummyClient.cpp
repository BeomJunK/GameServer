#include "pch.h"
#include <iostream>
#include "Session.h"
#include "Service.h"


char sendData[] = "Hello World";
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
		
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
		::memcpy(sendBuffer->Buffer(), sendData, sizeof(sendData));
		sendBuffer->Close(sizeof(sendData));
		
		Send(sendBuffer);
	}
	void OnSend(DWORD len) override
	{
		cout << "데이터  보냄" << len << endl;
	}
	
	int32 OnRecv(BYTE* buffer, int32 len) override
	{
		if (len == 0)
			return 0;
		cout << "데이터 받음" << len << endl;
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
		::memcpy(sendBuffer->Buffer(), sendData, sizeof(sendData));
		sendBuffer->Close(sizeof(sendData));
		
		this_thread::sleep_for(1s);
		Send(sendBuffer);
		
		return len;
	}
};
int main()
{
	this_thread::sleep_for(1s);
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