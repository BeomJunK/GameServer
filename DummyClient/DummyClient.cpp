#include "pch.h"
#include <iostream>
#include "Session.h"
#include "Service.h"


class ServerSession : public PacketSession
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
	
	void OnSend(DWORD len) override
	{
		cout << "데이터  보냄" << len << endl;
	}
	
	int32 OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketHeader header = *((PacketHeader*)&buffer[0]);
		cout << "Pakcet ID : " << header.id << "Size : " << header.size << endl;

		char recvBuffer[4096];
		::memcpy(recvBuffer, &buffer[4], header.size - sizeof(PacketHeader));
		cout << recvBuffer << endl;
		return len;
	}
};
int main()
{
	SetConsoleOutputCP(65001);

	this_thread::sleep_for(1s);
	ClientServiceRef service = MakeShared<ClientService>(
	NetAddress(L"127.0.0.1", 7777),
	MakeShared<IocpCore>(),
	MakeShared<ServerSession>,
	5
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