#include "pch.h"
#include <iostream>

#include "BufferReader.h"
#include "ClientPacketHandleler.h"
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
	
	void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		ClientPacketHandleler::HandlePacket(buffer, len);
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
	1
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