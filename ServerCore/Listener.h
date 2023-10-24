#pragma once

#include "IocpCore.h"
#include "IocpEvent.h"

class AcceptEvent;

class Listener : public IocpObject
{
public:
    Listener() = default;
    ~Listener();
public:
    bool StartAccept(NetAddress netAddress);
    void CloseSocket();

    virtual HANDLE GetHandle() override;
    virtual void Dispatch(class IocpEvent* iocpEvent, DWORD numOfBytes = 0) override;

private:
    void RegisterAccept(AcceptEvent* acceptEvent);
    void ProcessAccept(AcceptEvent* acceptEvent);
protected:
    SOCKET _socket = INVALID_SOCKET;
    Vector<AcceptEvent*> _acceptEvent;
};