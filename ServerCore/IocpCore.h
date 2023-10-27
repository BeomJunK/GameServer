#pragma once
/*-------------
	IocpObject
 -------------*/
class IocpObject : public enable_shared_from_this<IocpObject>
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

	bool Register(IocpObjectRef iocpObject);
	bool Dispatch(int32 timeOutMs = INFINITE);

private:
	HANDLE _iocpHandle;
};


