#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"


Listener::~Listener()
{
    SocketUtils::Close(_socket);

    for (auto acceptEvent : _acceptEvent)
    {
        xdelete(acceptEvent);
    }
}

bool Listener::StartAccept(NetAddress netAddress)
{
    _socket = SocketUtils::CreateSocket();
    if (_socket == INVALID_SOCKET)
        return false;

    if (GIocpCore.Register(this) == false)
        return false;

    if (SocketUtils::ReUseAddress(_socket, true) == false)
        return false;

    if (SocketUtils::SetLinger(_socket, 0, 0) == false)
        return false;

    if (SocketUtils::Bind(_socket, netAddress) == false)
        return false;

    if (SocketUtils::Listen(_socket) == false)
        return false;

    const int32 acceptCount = 1;

    for (int i = 0; i < acceptCount; i++)
    {
        AcceptEvent* acceptEvent = xnew<AcceptEvent>();
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
    ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);
    AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    Session* session = xnew<Session>();

    acceptEvent->Init();
    acceptEvent->SetSession(session);


    DWORD bytesReceived = 0;
    if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
    {
        const int32 errCode = ::WSAGetLastError();
        if(errCode != WSA_IO_PENDING)
        {
            //다시 accept상태로 만들어야 accept가 가능하다 (낚시대)
            RegisterAccept(acceptEvent);
        }
    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    Session* session = acceptEvent->GetSession();

    if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    SOCKADDR_IN sockAddress;
    int32 sizeofSockAddr = sizeof(sockAddress);
    if(SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeofSockAddr))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    session->SetNetAddress(NetAddress(sockAddress));

    wstring ip = session->GetAddress().GetIPAddress();
    wcout << L"connect client : " <<  ip << endl;
    
    RegisterAccept(acceptEvent);
}
