#include "pch.h"
#include "SocketUtils.h"
/*-------------------
     Socket Utils
 -------------------*/

LPFN_CONNECTEX          SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX       SocketUtils::DisConnectEx = nullptr;
LPFN_ACCEPTEX           SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init()
{
        /*윈도우즈에서 소켓 프로그래밍에서 제일 먼저 호출 하게 되는것이 WSAStartup 함수이다.
    이 함수는  UNIX 소켓 프로그램에는 없는것으로 WSACleanup 함수와 쌍을 이뤄 소켓 프로그램의
    시작과 끝을 나타낸다. 이 함수가 하는 일은 윈속 동적 연결 라이브러리를 초기화하고
    윈속 구현이 애플리케이션 요구사항을 충족하는지 확인한다. */
    WSADATA wsaData;
    ASSERT_CRASH(::WSAStartup(MAKEWORD(2,2), OUT &wsaData) == 0);


    /*런타임에 accept,connect함수주소 얻기위한 API호출*/
    SOCKET dummySock = CreateSocket();
    ASSERT_CRASH(BindWindowsFunction(dummySock, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
    ASSERT_CRASH(BindWindowsFunction(dummySock, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
    ASSERT_CRASH(BindWindowsFunction(dummySock, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisConnectEx)));

    Close(dummySock);
}

void SocketUtils::Clear()
{
    ::WSACleanup();
}

bool SocketUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
    //connect disconnect accept를 불러오기 위한 함수이다. 자세한 내용은 MSDN 참조
    DWORD bytes = 0;
    return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid,
        sizeof(guid), fn, sizeof(*fn), OUT &bytes, NULL, NULL);
}

SOCKET SocketUtils::CreateSocket()
{
    //::socket() 함수 대신 WSASocket()을 사용하면 세분화 설정 할 수 있다.
    //::socket() 함수는 WSA_FLAG_OVERLAPPED가 기본이다
   return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUtils::SetLinger(SOCKET socket, uint16 onOff, uint16 linger)
{
    LINGER option;
    option.l_onoff = onOff;
    option.l_linger = linger;
    return SetSocketOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtils::ReUseAddress(SOCKET socket, bool flag)
{
    return SetSocketOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtils::SetRecevBufferSize(SOCKET socket, int32 size)
{
    return SetSocketOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
    return SetSocketOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
    return SetSocketOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

bool SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET litenSocket)
{
    //listen Socket의 특성을 socket에 그대로 적용!
    return SetSocketOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, litenSocket);
}

bool SocketUtils::Bind(SOCKET socket, NetAddress netAddr)
{
    return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
    SOCKADDR_IN myAddress;
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
    myAddress.sin_port = htons(port);

    return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress)
        , sizeof(myAddress));
}

bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
    return SOCKET_ERROR != ::listen(socket, backlog);
}

void SocketUtils::Close(SOCKET& socket)
{
    if(socket != INVALID_SOCKET)
        ::closesocket(socket);

    socket = INVALID_SOCKET;
}
