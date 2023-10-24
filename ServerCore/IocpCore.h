#pragma once
/*-------------
	IocpObject
 -------------*/
class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, DWORD numOfBytes = 0) abstract;
};

/*-------------
	IocpCore
 -------------*/
class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE GetIocpHandle() { return _iocpHandle; }

	bool Register(class IocpObject* iocpObject);
	bool Dispatch(int32 timeOutMs = INFINITE);

private:
	HANDLE _iocpHandle;
};


//temp
extern IocpCore GIocpCore;

