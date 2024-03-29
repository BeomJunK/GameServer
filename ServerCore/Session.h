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
		BUFFER_SIZE = 0x10000, //64kb , 복사 방지를위해 10배 공간 할당한다.
	};
public:
	Session();
	virtual ~Session();

	/* Get Set */
	shared_ptr<Service> GetService() {return _service.lock();}
	void SetService(ServiceRef service){ _service = service;}
public:
	/* 외부 사용 */
	void Send(SendBufferRef sendBuffer);
	
	bool Connect();
	void Disconnect(const WCHAR* cause);
	
public:
	/* 정보 관련 */
	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }
	bool IsConnected() { return _connected; }
	SessionRef GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
private:
	/* 전송 관련 */
	bool RegisterConnect();
	bool RegisterDisConnect();
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessDisConnect();
	void ProcessRecv(DWORD numOfBytes);
	void ProcessSend(DWORD numOfBytes);

	void HandleError(int32 errorCode);

protected:
	/*컨텐츠 코드에서 구현*/
	virtual void OnConnected(){}
	virtual int32 OnRecv(BYTE* buffer, int32 len){return len;}
	virtual void OnDisconnected(){}
	virtual void OnSend(DWORD len) { }
	
	/* 인터페이스 구현 */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, DWORD numOfBytes = 0) override;

private:
	weak_ptr<Service> _service; //순환 막기위해 weak_ptr
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false;
private:
	USE_LOCK

	/* 수신 관련 */
	RecvBuffer _recvBuffer;

	/* 송신 관련 */
	Queue<SendBufferRef> _sendQueue;
	Atomic<bool> _sendRegistered = false;
private:
	/* iocpevent 재사용위해 */
	DisconnectEvent _disconnectEvent;
	ConnectEvent _connectEvent;
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
};

//패킷이 다오지 않을수 있는데 어떻게 판단할것인가 ?
//헤더를 붙여서 판단한다. 최소 4바이트는 받게하고 앞에 헤더를 어떤정보가있는지 파싱해본다.
/*------------------
	PacketSession  
-------------------*/
struct PacketHeader
{
	uint16 size;
	uint16 id; //프로토콜 ID
};

class PacketSession : public Session
{
public:
	PacketSession(){}
	virtual ~PacketSession(){}

	PacketSessionRef GetPacketSessionRef() {return static_pointer_cast<PacketSession>(shared_from_this());};

	virtual int32 OnRecv(BYTE* buffer, int32 len) sealed; //sealed : 상속받은애는 이걸 사용못하게 막음
	virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract;
};