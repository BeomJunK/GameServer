#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <memory>
#include <windows.h>
#include <future>
#include "ThreadManager.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	//윈속 초기화 (ws2_32 라이브러리 초기화)
	//관련 정보가 wsaData에 채워짐
	//실패시 0이 아닌값이 뜸
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2 , 2) , &wsaData) != 0)
		return 0;

	//안내원의 핸드폰
	//af : IPv4를 사용할건지 IPv6 를 사용할건지) / AF_INET = IPv4, AF_INET6 = IPv6
	//type :  TCP(SOCK_STREAM) , UDP(SOCK_DGRAM) 선택
	//protocol : 0으로 세팅
	//return : descriptor 소켓 번호
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if(listenSocket == INVALID_SOCKET)
	{
		//에러 몇번으로 실패했는지 받을 수 있다.
		int32 errCode = ::WSAGetLastError();
		cout << "Create Socket Failed Error : " << errCode << endl;
		return 0;
	}

	//나의 주소는 어디? (ip주소 + 포트)
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); //알아서 골라줘
	serverAddr.sin_port = ::htons(7777);

	//안내원의 폰 개통 (식당번호)
	if(::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		//에러 몇번으로 실패했는지 받을 수 있다.
		int32 errCode = ::WSAGetLastError();
		cout << "bind Failed Error : " << errCode << endl;
		return 0;
	}
	//영업 시작
	//대기열 10개 넘으면 바로 끊음
	if(::listen(listenSocket, 10) == SOCKET_ERROR)
	{
		//에러 몇번으로 실패했는지 받을 수 있다.
		int32 errCode = ::WSAGetLastError();
		cout << "Listen Error : " << errCode << endl;
		return 0;
	}

	// -------영업중-------
	while(true)
	{
		SOCKADDR_IN clientAddr; //Ipv4
		::memset(&clientAddr , 0, sizeof(clientAddr));

		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

		//손님 입장!
		char ipAddress[16];
		//ulng ip 문자열로 바꾸기
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));

		//TODO
		cout << "클라입장 : " << ipAddress << endl;
		while (true)
		{
			char recvBuffer [1024];
			int32 recvLen = ::recv(clientSocket , recvBuffer , sizeof(recvBuffer) , 0);
			if (recvLen <= 0)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Recv Error : " << errCode << endl;
				return 0;
			}

			cout << "데이터 받음 : " << recvBuffer << endl;

			int32 resultCode = ::send(clientSocket , recvBuffer , sizeof(recvBuffer) , 0);
			if (resultCode == SOCKET_ERROR)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Send Error : " << errCode << endl;
				return 0;
			}
			cout << "데이터 보냄" << endl;
		}
	}
	// --------------
	
	

	
	::WSACleanup();
}


