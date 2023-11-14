#include "pch.h"
#include <iostream>
#include "Session.h"
#include "Service.h"
#include "ServerPacketHandler.h"

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
		PacketSessionRef session = GetPacketSessionRef();

		//정상적인지 확인 필요
		//PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer, len);
		
		ServerPacketHandler::HandlePacket(session, buffer, len);
	}
};
int main()
{
	ServerPacketHandler::Init();

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