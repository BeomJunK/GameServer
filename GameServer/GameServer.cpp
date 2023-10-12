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

void HandleError(const char* cause)
{
	//에러 몇번으로 실패했는지 받을 수 있다.
	int32 errCode = ::WSAGetLastError();
	cout << "Create Socket Failed Error : " << errCode << endl;
}
int main()
{
	//윈속 초기화 (ws2_32 라이브러리 초기화)
	//관련 정보가 wsaData에 채워짐
	//실패시 0이 아닌값이 뜸
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2 , 2) , &wsaData) != 0)
		return 0;

	SOCKET serverSocket = ::socket(AF_INET , SOCK_DGRAM , 0);
	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}



	//연결 상태 체크
	bool enable = true;
	::setsockopt(serverSocket , SOL_SOCKET , SO_KEEPALIVE , (char*)enable , sizeof(enable));

	//종료 할떄 송신버퍼 보내고 종료할것인가? 아니면 버릴것인가?
	//0이면 closesocket이 바로 리턴 아니면 linger초만큼 대기 (default 0)
	linger linger;
	linger.l_onoff = 1; //on
	linger.l_linger = 5; //5초간 대기
	::setsockopt(serverSocket , SOL_SOCKET , SO_LINGER , (char*)&linger , sizeof(linger));
	

	//정석적으로 끄는방법 closesocket전 매너있게 끊는법 shutdown
	//Half-Close 
	//SD_SEND : send 막는다
	//SD_RECEIVE : recv 막는다
	//SD_BOTH : 둘다 막는다
	/*::shutdown(serverSocket , SD_BOTH);
	::closesocket(serverSocket);*/


	//SO_SNDBUF,SO_RCVBUF : 송수신 버퍼 크기 확인
	int32 sendBufferSize;
	int setSize = 85555;
	int32 optionSize = sizeof(sendBufferSize);
	//버퍼사이즈 설정가능
	::setsockopt(serverSocket , SOL_SOCKET , SO_SNDBUF , (char*)&setSize , sizeof(setSize));
	::getsockopt(serverSocket , SOL_SOCKET , SO_SNDBUF , (char*)&sendBufferSize , &optionSize);
	cout << "send buff size : " << sendBufferSize << endl;

	int32 recvBufferSize;
	optionSize = sizeof(sendBufferSize);
	::getsockopt(serverSocket , SOL_SOCKET , SO_RCVBUF , (char*)&recvBufferSize , &optionSize);
	cout << "recv buff size : " << recvBufferSize << endl;

	//SO_SENDBUF : IP주소 포트 재사용
	//프로그램 종료후 바인딩된 ip port가 남아있다면 다시 서버를 뛰울수 없을 것이다.
	//이런걸 예방하기위해 재사용하겠다고 말하는것이다.
	{
		bool enable = true;
		::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable , sizeof(enable));
	}

	//IPPROTO_TCP
	//네이글 알고리즘 작동여부
	//데이터가 충분하면 보내고 아니면 충분히 쌓일때 까지 기다린다.
	//장점 : 작은패킷이 불필요하게 많이 생성되는것 방지
	//단점 : 반응 시간 손해
	//게임에선 반응속도가 빨라야하기때문에 대부분에서 꺼준다.
	{
		bool enable = true;
		::setsockopt(serverSocket , IPPROTO_TCP , TCP_NODELAY , (char*)&enable , sizeof(enable));
	}

	::WSACleanup();
}


