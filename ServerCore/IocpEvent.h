#pragma once

enum class EventType : uint8
{
    Connect,
    DisconnectEvent,
    Accept,
    Recv,
    Send
};

class Session;

/*----------------
    IocpEvent
-----------------*/
class IocpEvent : public OVERLAPPED
{
public:
    IocpEvent(EventType type);

    void Init();
public:
    EventType eventType;

    //IocpObject를
    //event를 주고받는 와중에 삭제 할수도 있으니
    //레퍼런스 카운팅해 사라지지 않도록 한다.
    IocpObjectRef owner;
};


/*----------------
    ConnectEvent
-----------------*/
class ConnectEvent : public IocpEvent
{
public:
    ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*----------------
    DisconnectEvent
-----------------*/
class DisconnectEvent : public IocpEvent
{
public:
    DisconnectEvent() : IocpEvent(EventType::DisconnectEvent) { }
};

/*----------------
    AcceptEvent
-----------------*/
class AcceptEvent : public IocpEvent
{
public:
    AcceptEvent() : IocpEvent(EventType::Accept) { }

   
public:
    SessionRef session = nullptr;
    
};
/*----------------
    RecvEvent
-----------------*/
class RecvEvent : public IocpEvent
{
public:
    RecvEvent() : IocpEvent(EventType::Recv) { }
};
/*----------------
    SendEvent
-----------------*/
class SendEvent : public IocpEvent
{
public:
    SendEvent() : IocpEvent(EventType::Send) { }

    Vector<SendBufferRef> buffer;
};