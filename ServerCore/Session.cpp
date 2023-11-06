#include "pch.h"
#include "Session.h"
#include "Service.h"
#include "SocketUtils.h"

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

    wcout << "Discconect : " << cause << endl;

   

    RegisterDisConnect();
}

bool Session::RegisterConnect()
{
    if(IsConnected())
        return false;

    if(GetService()-> GetServiceType() != ServiceType::Client)
        return false;

    if(SocketUtils::ReUseAddress(_socket,TRUE) == false)
        return false;

    if(SocketUtils::BindAnyAddress(_socket,/* 남는거 */0) == false)
        return false;

    _connectEvent.Init();
    _connectEvent.owner = shared_from_this();

    DWORD numOfBytes = 0;
    SOCKADDR_IN sockAddr = GetService()->GetNetworkAddress().GetSockAddr();
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

void Session::RegisterRecv()
{
    //연결끊은놈껀 받지않도록
    if (IsConnected() == false)
        return;

    _recvEvent.Init();
    _recvEvent.owner = shared_from_this();

    WSABUF wsabuf;
    wsabuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
    wsabuf.len = _recvBuffer.FreeSize();

    DWORD numOfBytes = 0;
    DWORD flags = 0;
    if (SOCKET_ERROR == ::WSARecv(_socket, &wsabuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            HandleError(errorCode);
            _recvEvent.owner = nullptr; // RELEASE_REF
        }
    }
}

void Session::ProcessConnect()
{
    _connectEvent.owner = nullptr;
    _connected.store(true);

    //세션 등록
    GetService()->AddSession(GetSessionRef());

    // 컨텐츠 코드에서 오버로딩한것 실행
    OnConnected();

    //수신 등록
    //한번은 걸어줘야 받기 시작한다.
    //또한 한번받고(pending포함) 다시 호출해줘야한다.
    RegisterRecv();
}

void Session::ProcessRecv(DWORD numOfBytes)
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
	
    // 수신 등록 (낚시대)
    RegisterRecv();
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
        //TODO : Log
        cout << "Handle Error: " << errorCode << endl;
        break;
    }
}

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket); 
}
bool Session::RegisterDisConnect()
{
    _disconnectEvent.Init();
    _disconnectEvent.owner = shared_from_this();

    if(false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
    {
        int32 errCode = ::WSAGetLastError();
        if(errCode != WSA_IO_PENDING)
        {
            _disconnectEvent.owner = nullptr;// RELEASE_REF
            return false;
        }
    }
    return true;
}
void Session::ProcessDisConnect()
{
    OnDisconnected();
    GetService()->ReleaseSession(GetSessionRef());

    _disconnectEvent.owner = nullptr;// RELEASE_REF
}


void Session::Send(SendBufferRef sendBuffer)
{
    if(IsConnected() == false)
        return;

    bool registerSend = false;
    
    {
        WRITE_LOCK
        //만약 누군가 보내고있따면 queue에 쌓아만 둔다.
        _sendQueue.push(sendBuffer);
        if (_sendRegistered.exchange(true) == false)
            registerSend = true;
    }


    if (registerSend)
        RegisterSend();
}

void Session::RegisterSend()
{
    if(IsConnected() == false)
        return;

    _sendEvent.Init();
    _sendEvent.owner = shared_from_this();

    //큐에 쌓인 버퍼 sendEvent에 등록
    //여기서 여러 쓰레드에 의해 이벤트애 버퍼 등록됨
    {
        WRITE_LOCK

        int32 writeSize = 0;//쌓인 SendBuffer의 총 크기 SendBuffer * n
        while (_sendQueue.empty() == false)
        {
            SendBufferRef sendBuffer = _sendQueue.front();

            //너무 많은데이터 보내는것을 방지하기위해
            //보낸 데이터 기록
            writeSize += sendBuffer->WriteSize();
            //TODO : if 많이보냈다면 예외처리 
            
            _sendQueue.pop();
            _sendEvent.buffer.push_back(sendBuffer);
        }
    }

    //Scatter - Gather 흩어져있는데이터를 한번에 보내는 기법
    Vector<WSABUF> wsaBufs;
    wsaBufs.reserve(_sendEvent.buffer.size());

    //이벤트안에 여러 버퍼들이 들어있을것이다.
    for(SendBufferRef sendBuffer : _sendEvent.buffer)
    {
        //WSABUF를  이벤트안에 버퍼만큼 만들어줘서 벡터에 저장해논다
        WSABUF wsaBuf;
        wsaBuf.buf = (char*)sendBuffer->Buffer();
        wsaBuf.len = (ULONG)sendBuffer->WriteSize();
        wsaBufs.push_back(wsaBuf);
    }
   
    DWORD numOfBytes = 0;
    if(SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT &numOfBytes, 0, &_sendEvent, nullptr ))
    {
        int32 errCode = ::WSAGetLastError();
        if(errCode != WSA_IO_PENDING)
        {
            HandleError(errCode);
            _sendEvent.owner = nullptr;
            _sendEvent.buffer.clear();
            _sendRegistered.store(false);
        }
    }
}
void Session::ProcessSend(DWORD numOfBytes)
{
    // RELEASE_REF : ProcessSend가 실행되는 시점엔 이벤트의버퍼는 모두 처리가 됐다. clear로 비워주기
    _sendEvent.buffer.clear(); 
    _sendEvent.owner = nullptr;// RELEASE_REF
   
    
    if(numOfBytes == 0)
    {
        Disconnect(L"Send 0");
    }
	
    //콘텐츠에서 구현
    OnSend(numOfBytes);

    WRITE_LOCK
    if(_sendQueue.empty())
        _sendRegistered.store(false);
    else
        RegisterSend();
    
}
bool Session::Connect()
{
    return RegisterConnect();
}
void Session::Dispatch(IocpEvent* iocpEvent, DWORD numOfByte)
{
    switch (iocpEvent->eventType)
    {
    case EventType::Connect:
        ProcessConnect();
        break;
    case EventType::DisconnectEvent:
        ProcessDisConnect();
        break;
    case EventType::Recv:
        ProcessRecv(numOfByte);
        break;
    case EventType::Send:
        ProcessSend(numOfByte);
        break;
    default:
        break;
    }
}
/*------------------
    PacketSession  
-------------------*/




int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
    int32 processLen = 0;

    while(true)
    {
        int32 dataSize = len - processLen;
        if(dataSize < sizeof(PacketHeader))
            break;

        //헤더 꺼내기
        PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));

        //헤더에 기록된 패킷 크기를 파싱할수 있어야함
        if(dataSize < header.size)
            break;

        //패킷 조립 성공
        OnRecvPacket(&buffer[processLen], header.size);

        processLen += header.size;
    }
    
    return processLen;
}

