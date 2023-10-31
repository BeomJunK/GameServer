#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

/*--------------
	Session
---------------*/

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}



void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	// TEMP
	wcout << "Disconnect : " << cause << endl;

	OnDisconnected(); // 컨텐츠 코드에서 오버로딩
	GetService()->ReleaseSession(GetSessionRef());

	RegisterConnect();
}

bool Session::Connect()
{
	return RegisterConnect();
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::DisConnectEvent:
		ProcessDisConnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
		break;
	default:
		break;
	}
}

bool Session::RegisterConnect()
{
	if(IsConnected())
		return false;

	if(GetService()-> GetServiceType() != ServiceType::Client)
		return false;

	if(SocketUtils::SetReuseAddress(_socket,TRUE) == false)
		return false;

	if(SocketUtils::BindAnyAddress(_socket,/*남는거*/0) == false)
		return false;

	_connectEvent.Init();
	_connectEvent.owner = shared_from_this();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if( false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr),
		 sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	{
		int32 errCode = ::WSAGetLastError();
		if(errCode != WSA_IO_PENDING)
		{
			_connectEvent.owner = nullptr;
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisConnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this();

	if(false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errCode = ::WSAGetLastError();
		if(errCode == WSA_IO_PENDING)
		{
			_disconnectEvent.owner = nullptr;
			return false;
		}
	}
	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this(); // ADD_REF

	// TCP에서는 전송 바운더리가 없기때문에 100바이트를 보냈을떄
	//한번 받았을때 정확히 100바이트를 한번에 받을거란 보장이 없다.
	//완전체로 받지 않았다면 데이터를 덫붙여야 된다.
	
	
	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos()); //이전에 받은곳의 다음위치
	wsaBuf.len = _recvBuffer.FreeSize(); //버퍼의 최대크가

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr; // RELEASE_REF
		}
	}
}
void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr; // RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	//다음커서의 위치
	if(_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite OverFlow");
		return;
	}

	//여기까지 왔다는건 커널 버퍼 -> session버퍼로 데이터 복사 이루어졌다는 이야기
	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
	if ((processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen)) == false)
	{
		Disconnect(L"OnRead OverFlow");
		return;
	}

	//커서 정리
	_recvBuffer.Clean();
	
	// 수신 등록
	RegisterRecv();
}

void Session::Send(BYTE* buffer, int32 len)
{
	SendEvent* sendEvent = xnew<SendEvent>();
	sendEvent->owner = shared_from_this();
	sendEvent->buffer.resize(len);
	::memcpy(sendEvent->buffer.data(), buffer, len);

	WRITE_LOCK
	RegisterSend(sendEvent);
}
void Session::RegisterSend(SendEvent* sendEvent)
{
	//생각해봐야할 문제
	
	//연속적으로 보내개 된다면 가능한가?
	//WSASend는 멀티 스레드 환경에서 Safe하지 않다.
	//어떤식으로건 순서를 보장 해줘야한다

	//보내더라도 순서가 섞여버릴수있음

	//버퍼가 꽉 찼다면 굳이 데이터를 밀어넣을 필요가 없다.

	
	if(IsConnected() == false)
		return;

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->buffer.data();
	wsaBuf.len = (ULONG)sendEvent->buffer.size();

	DWORD numOfBytes = 0;
	if(SOCKET_ERROR == ::WSASend(_socket, &wsaBuf, 1, OUT &numOfBytes, 0, sendEvent, nullptr ))
	{
		int32 errCode = ::WSAGetLastError();
			if(errCode != WSA_IO_PENDING)
			{
				HandleError(errCode);
				sendEvent->owner = nullptr;
				xdelete(sendEvent);
			}
	}
}

void Session::ProcessConnect()
{
	_connectEvent.owner = nullptr;
	
	_connected.store(true);

	// 세션 등록
	GetService()->AddSession(GetSessionRef());

	// 컨텐츠 코드에서 오버로딩
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessDisConnect()
{
	_disconnectEvent.owner = nullptr;

	
}


void Session::ProcessSend(SendEvent* sendEvent, int32 numOfBytes)
{
	sendEvent->owner = nullptr;
	xdelete(sendEvent);

	if(numOfBytes == 0)
	{
		Disconnect(L"Send 0");
	}
	
	//콘텐츠에서 구현
	OnSend(numOfBytes);

	
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		cout << "Handle Error : " << errorCode << endl;
		break;
	}
}