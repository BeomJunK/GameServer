#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"


Listener::~Listener()
{
    SocketUtils::Close(_socket);

    for (auto acceptEvent : _acceptEvent)
    {
        xdelete(acceptEvent);
    }
}

bool Listener::StartAccept(ServerServiceRef service)
{
    _service = service;
    if(_service == nullptr)
        return false;

    
    _socket = SocketUtils::CreateSocket();
    if (_socket == INVALID_SOCKET)
        return false;

    if (_service->GetIocpCore()->Register(shared_from_this()) == false)
        return false;

    if (SocketUtils::ReUseAddress(_socket, true) == false)
        return false;

    if (SocketUtils::SetLinger(_socket, 0, 0) == false)
        return false;

    if (SocketUtils::Bind(_socket, _service->GetNetworkAddress()) == false)
        return false;

    if (SocketUtils::Listen(_socket) == false)
        return false;

    const int32 acceptCount = _service->GetMaxSessionCount();

    for (int i = 0; i < acceptCount; i++)
    {
        AcceptEvent* acceptEvent = xnew<AcceptEvent>();
        acceptEvent->owner = shared_from_this();
        _acceptEvent.push_back(acceptEvent);
        RegisterAccept(acceptEvent);
    }


    return true;
}

void Listener::CloseSocket()
{
    SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, DWORD numOfBytes)
{
    // 일거리 없는 스레드가 일감 받아옴
    ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);
    AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    SessionRef session =  _service->CreateSession(); // 세션 생성후, 여기서 Iocp에 등록!!!! 

    acceptEvent->Init();
    acceptEvent->session = session;


    DWORD bytesReceived = 0;

    //Iocp에일감 던지기 나중에
    //일거리 없는 스레드가 와서 일감을 가져갈것임
    if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
    {
        //일감 던졌는데 아직실행이 되지않았네?
        //다시 RegisterAccept걸어주기 (iocp에 일감 다시던지기)
        const int32 errCode = ::WSAGetLastError();
        if(errCode != WSA_IO_PENDING) 
        {
            //다시 accept상태로 만들어야 accept가 가능하다 (낚시대 던지기)
            RegisterAccept(acceptEvent);
        }
        else
        {
            // TODO : Error 상황
        }
    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    IocpEvent* iocpEvent = static_cast<IocpEvent*>(acceptEvent);
    ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);

    SessionRef session = acceptEvent->session;

    if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    //SOCKADDR_IN 가져오기
    SOCKADDR_IN sockAddress;
    int32 sizeofSockAddr = sizeof(sockAddress);
    if(SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeofSockAddr))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    //Address 세팅
    session->SetNetAddress(NetAddress(sockAddress));
    session->ProcessConnect();
   
    
    RegisterAccept(acceptEvent);

    
}
