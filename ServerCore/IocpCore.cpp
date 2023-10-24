#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore GIocpCore;
/*-------------
	IocpCore
 -------------*/
IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObject* iocpObject)
{
	//관찰 대상에 등록
	//HANDLE이 소켓이 아닌 다른객체로 활용할수 있기때문에 iocpObject로 만들어 관리한다.
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle,
			reinterpret_cast<ULONG_PTR>(iocpObject), 0);
}

bool IocpCore::Dispatch(int32 timeOutMs)
{
	//일감 꺼내오기
	DWORD numOfBytes = 0;
	IocpObject* iocpObject =nullptr;
	IocpEvent* iocpEvent =  nullptr;

	//일감이 없다면 쓰레드는 잠들고 일감이 들어왔을때 쓰레드를 깨운다.
	if( ::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfBytes, /*key*/OUT reinterpret_cast<PULONG_PTR>(&iocpObject), OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeOutMs))
	{
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch(errCode)
		{
		case WSA_WAIT_TIMEOUT:
			return false;
		default:
			//오류 상황 , 로그 남기기
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}
}


