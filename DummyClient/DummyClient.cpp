#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <MSWSock.h>
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	//에러 몇번으로 실패했는지 받을 수 있다.
	int32 errCode = ::WSAGetLastError();
	cout << cause << errCode << endl;
}
int main()
{
	this_thread::sleep_for(1s);
	//윈속 초기화 (ws2_32 라이브러리 초기화)
//관련 정보가 wsaData에 채워짐
//실패시 0이 아닌값이 뜸

	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2 , 2) , &wsaData) != 0)
		return 0;

	SOCKET clientSocket = ::socket(AF_INET , SOCK_STREAM , 0);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(clientSocket , FIONBIO , &on) == INVALID_SOCKET)
		return 0;

	//Server Addr
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr , 0 , sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET , "127.0.0.1" , &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	//Connect
	while (true)
	{
		if (::connect(clientSocket , (SOCKADDR*)&serverAddr , sizeof(serverAddr)) == SOCKET_ERROR)
		{
			//요청이 없는상황 원래는 블록이지만 논블로킹으로 만들었음
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			//이미 연결된 상태
			if (::WSAGetLastError() == WSAEISCONN)
				break;

			//error
			break;
		}
	}

	cout << "Connected server!" << endl;

	char sendBuffer [100] = "Hello world!";
	//send
	while (true)
	{
		

		if (::send(clientSocket , sendBuffer , sizeof(sendBuffer) , 0) == SOCKET_ERROR)
		{
			//요청이 없는상황 원래는 블록이지만 논블로킹으로 만들었음
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			//error
			break;
		}

		cout << "Send data Len :" << sizeof(sendBuffer) << endl;

		while (true)
		{
			char recvBuffer [100];
			int32 recvLen = ::recv(clientSocket , recvBuffer , sizeof(recvBuffer) , 0);
			if (recvLen == SOCKET_ERROR)
			{
				//요청이 없는상황 원래는 블록이지만 논블로킹으로 만들었음
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;

				//error
				break;
			}
			else if (recvLen == 0)
			{
				//연결 끊김
				break;
			}

			cout << "Recv Data Len : " << recvLen << endl;
			break;
		}
		this_thread::sleep_for(1s);
	}

	

	::WSACleanup();
}