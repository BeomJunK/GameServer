#pragma once
#include "NetAddress.h"

 /*-------------------
      Socket Utils
  -------------------*/
class SocketUtils
{
public:
    //connect, disconnect, accept를 비동기로 호풀하기 위한
    //함수 포인터 - 런타임중에 가져와야 한다.
    static LPFN_CONNECTEX           ConnectEx;
    static LPFN_DISCONNECTEX        DisconnectEx;
    static LPFN_ACCEPTEX            AcceptEx;

public:
    static void Init();
    static void Clear();

    static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
    static SOCKET CreateSocket();

    static bool SetLinger(SOCKET socket, uint16 onOff, uint16 linger);
    static bool ReUseAddress(SOCKET socket, bool flag);
    static bool SetRecevBufferSize(SOCKET socket, int32 size);
    static bool SetSendBufferSize(SOCKET socket, int32 size);
    static bool SetTcpNoDelay(SOCKET socket, bool flag);
    static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET litenSocket);
    
    static bool Bind(SOCKET socket, NetAddress netAddr);
    static bool BindAnyAddress(SOCKET socket, uint16 port);
    static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
    static void Close(SOCKET& socket);

    template <typename T>
    static inline bool SetSocketOpt(SOCKET socket, int32 level, int32 optName, T optVal)
    {
        return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
    }
};

