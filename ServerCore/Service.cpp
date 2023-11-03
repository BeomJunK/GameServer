#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

/*---------------
      Service
----------------- */
Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
      :_type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{
      
}


Service::~Service()
{
    
}

bool Service::Start()
{
      return true;
}

void Service::CloseService()
{
      //TODO
      
}

SessionRef Service::CreateSession()
{
      SessionRef session = _sessionFactory();
      session->SetService(shared_from_this());
      
      if(_iocpCore->Register(session) == false)
            return nullptr;

      return session;
}

void Service::AddSession(SessionRef session)
{
      WRITE_LOCK
      _sessionCount++;
      _sessions.insert(session);
}

void Service::ReleaseSession(SessionRef session)
{
      WRITE_LOCK
      ASSERT_CRASH(_sessions.erase(session) != 0);
      _sessionCount--;
}

/*----------------------
     Client Service
------------------------ */

ClientService::ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
      :Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount )
{
      
}

bool ClientService::Start()
{
    if (CanStart() == false)
        return false;

    const int32 sessionCount = GetMaxSessionCount();
   
    //���� �����׽�Ʈ
    //��Ʈ���� �׽�Ʈ �Ҷ��� ���� ������ ����� �����Ų��.
    for (int i = 0; i < sessionCount; i++)
    {
        SessionRef session = CreateSession();
        if (session->Connect() == false)
            return false;
    }


    return true;
}
/*----------------------
      Server Service
------------------------ */
ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
:Service(ServiceType::Client, address, core, factory, maxSessionCount )
{
      
}

bool ServerService::Start()
{
      if(CanStart() == false)
            return false;

      _listner = MakeShared<Listener>();
      if(_listner == nullptr)
            return false;

      ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
      if(_listner->StartAccept(service) == false)
            return false;

      
      return true;
}

void ServerService::CloseService()
{
      //TODO
      
      Service::CloseService();
}


