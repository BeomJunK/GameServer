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
	}
	
	void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketSessionRef session = GetPacketSessionRef();

		ServerPacketHandler::HandlePacket(session, buffer, len);
	}
	void OnConnected() override
	{
		Protocol::C_LOGIN pkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		Send(sendBuffer);
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

	Protocol::C_CHAT pkt;
	pkt.set_msg(u8"Hello");
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	
	while(true)
	{
		service->Broadcast(sendBuffer);
		this_thread::sleep_for(1s);
		
	}

	GThreadManager->Join();
}