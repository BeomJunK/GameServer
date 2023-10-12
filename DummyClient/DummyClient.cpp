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
	//윈속 초기화 (ws2_32 라이브러리 초기화)
//관련 정보가 wsaData에 채워짐
//실패시 0이 아닌값이 뜸

	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2 , 2) , &wsaData) != 0)
		return 0;


	//통신 단말기 생성!
	//af : IPv4를 사용할건지 IPv6 를 사용할건지) / AF_INET = IPv4, AF_INET6 = IPv6
	//type :  TCP(SOCK_STREAM) , UDP(SOCK_DGRAM) 선택
	//protocol : 0으로 세팅
	//return : descriptor 소켓 번호
	SOCKET clientSocket = ::socket(AF_INET , SOCK_DGRAM , 0);
	if (clientSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	//식당 전화번호 입력
	//연결할 목적지는 어디? (ip주소 + 포트)
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr , 0 , sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;

	//serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); //구시대적 방법이라 경고
	::inet_pton(AF_INET , "127.0.0.1" , &serverAddr.sin_addr);

	// host to network short (htons)로 저장하는 이유
	// host에서 네트워크 방식으로 바꿔주겠다.
	// cpu에 따라 LittelEndian과 BigEndian 저장방식이 다르다고 한다.
	// 네트워크에서 표준은 BigEndian 방식이다. 표준으로 바꿔줘야한다
	serverAddr.sin_port = ::htons(7777);

	//Connedted UDP
	::connect(clientSocket , (SOCKADDR*)&serverAddr , sizeof(serverAddr));

	//이제 연결성공
	while (true)
	{
		char sendBuffer [100] = "Hello World!";

		//Unconnected UDP
		/*int32 resultCode = ::sendto(clientSocket , sendBuffer , sizeof(sendBuffer) , 0
			, (SOCKADDR*)&serverAddr , sizeof(serverAddr));*/

		//Connected UDP
		int32 resultCode = ::send(clientSocket , sendBuffer , sizeof(sendBuffer) , 0);

		if (resultCode == SOCKET_ERROR)
		{
			HandleError("SendTo");
			return 0;
		}

		cout << "send len : " << sizeof(sendBuffer) << endl;

		SOCKADDR_IN recvAddr;
		::memset(&recvAddr , 0 , sizeof(recvAddr));
		int32 addrLen = sizeof(recvAddr);

		char recvBuffer [1024];
		//Unconnected UDP
		/*int32 recvLen = ::recvfrom(clientSocket , recvBuffer , sizeof(recvBuffer) , 0
			, (SOCKADDR*)&recvAddr , &addrLen);*/

		//Connected UDP
		int32 recvLen = ::recv(clientSocket , recvBuffer , sizeof(recvBuffer) , 0);
		if (recvLen <= 0)
		{
			HandleError("RecvFrom");
			return 0;
		}

		cout << "recv data : " << recvBuffer << endl;
		cout << "recv len : " << recvLen << endl;



		this_thread::sleep_for(1ms);
	}

	::closesocket(clientSocket);
	::WSACleanup();
}