#include "pch.h"
#include "ClientPacketHandler.h"

#include "GameSession.h"
#include "GameSessionManager.h"
#include "Player.h"
#include "Room.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}
bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	//TODO : Validation 체크
	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	//TODO : DB에서 플레이어 정보 긁어오기
	//GameSession에 플레이 정보저장 (DB에있는걸 매모리에 저장한다)

	//임시 ID발급(나중에 DB에 저장된 ID로 발급해줄것)
	static Atomic<uint64> idGenerator = 1;
	{
		auto player = loginPkt.add_players();
		player->set_name(u8"db에서 긁어온 이름");
		player->set_playertype(Protocol::PLAYER_TYPE_KNIGHT);

		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->_players.push_back(playerRef);
	}
	{
		auto player = loginPkt.add_players();
		player->set_name(u8"db에서 긁어온 이름2");
		player->set_playertype(Protocol::PLAYER_TYPE_ARCHER);

		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->_players.push_back(playerRef);
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	session->Send(sendBuffer);
	
	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	uint64 index = pkt.playerindex();

	//TODO : Validation 클라에서 보낸건 절대 신뢰x

	//read only로 쓸거기때문에 스레드safe 하다고 가정
	PlayerRef player = gameSession->_players[index];

	GRoom.PushJob(&Room::Enter, player);

	Protocol::S_ENTER_GAME enterPkt;
	enterPkt.set_success(true);
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterPkt);
	session->Send(sendBuffer);
	
	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	cout << pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);
	GRoom.PushJob(&Room::Broadcast, sendBuffer);
	return true;
}






