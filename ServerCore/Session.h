#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

/*--------------
	Session
---------------*/

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, //64kb , ���� ���������� 10�� ���� �Ҵ��Ѵ�.
	};
public:
	Session();
	virtual ~Session();

public:
                        /* �ܺ� ��� */
	void				Send(BYTE* buffer, int32 len);
	void				Disconnect(const WCHAR* cause);
	bool				Connect();

	shared_ptr<Service>	GetService() { return _service.lock(); }
	void				SetService(shared_ptr<Service> service) { _service = service; }

public:
						/* ���� ���� */
	void				SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress			GetAddress() { return _netAddress; }
	SOCKET				GetSocket() { return _socket; }
	bool				IsConnected() { return _connected; }
	SessionRef			GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
						/* �������̽� ���� */
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
						/* ���� ���� */
	bool				RegisterConnect();
	bool				RegisterDisConnect();
	void				RegisterRecv();
	void				RegisterSend(SendEvent* sendEvent);

	void				ProcessConnect();
	void				ProcessDisConnect();
	void				ProcessRecv(int32 numOfBytes);
	void				ProcessSend(SendEvent* sendEvent, int32 numOfBytes);

	void				HandleError(int32 errorCode);

protected:
						/* ������ �ڵ忡�� �����ε� */
	virtual void		OnConnected() { }
	virtual int32		OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void		OnSend(int32 len) { }
	virtual void		OnDisconnected() { }

private:
	weak_ptr<Service>	_service;
	SOCKET				_socket = INVALID_SOCKET;
	NetAddress			_netAddress = {};
	Atomic<bool>		_connected = false;

private:
	USE_LOCK;

	/* ���� ���� */
	RecvBuffer _recvBuffer;
	/* �۽� ���� */

private:
						/* IocpEvent ���� */
	DisConnectEvent		_disconnectEvent;
	ConnectEvent		_connectEvent;
	RecvEvent			_recvEvent;
};
