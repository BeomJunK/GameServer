#include "pch.h"

#include "BufferWriter.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandleler.h"

int main()
{
	 SetConsoleOutputCP(65001);
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

	char sendData[] = "Hello World!";
	while(true)
	{
		Vector<BuffData> buffs{ BuffData{100, 1.5f},BuffData{200, 2.5f},BuffData{300, 3.5f}};
		
		SendBufferRef sendBuffer = ServerPacketHandleler::Make_SC_TEST(1001, 100, 10, buffs);
		this_thread::sleep_for(250ms);
		GSessionManager.Broadcast(sendBuffer);
	}

	GThreadManager->Join();
}


