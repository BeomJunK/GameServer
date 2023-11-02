#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>


//서버와 서버간 통신을 하거나
//세션을 확장성 있게 만들때 편하게하기 위해
//Service로 관리하면 좋다.

enum ServiceType
{
      Client,
      Server
};
/*---------------
      Service
----------------- */
using SessionFactory = function<SessionRef(void)>;

class Service : public enable_shared_from_this<Service>
{
public:
      Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
      virtual ~Service();

      virtual bool Start() abstract;
      bool CanStart() {return _sessionFactory != nullptr;}

      virtual void CloseService();
      void SetSessionFactory(SessionFactory func){_sessionFactory = func;}

      SessionRef CreateSession();
      void AddSession(SessionRef session);
      void ReleaseSession(SessionRef session);
      int32 GetCurrentSessionCount(){return _sessionCount;}
      int32 GetMaxSessionCount(){return _maxSessionCount;}

public:
      ServiceType GetServiceType() {return _type;}
      NetAddress GetNetworkAddress(){return _netAddress;}
      IocpCoreRef& GetIocpCore(){return _iocpCore;}
protected:
      USE_LOCK

      ServiceType _type;
      NetAddress _netAddress; //상대방일수도 있고 내꺼일수도 있고
      IocpCoreRef _iocpCore;

      Set<SessionRef> _sessions;
      int32 _sessionCount = 0;
      int32 _maxSessionCount = 0;
      SessionFactory _sessionFactory;
};

/*----------------------
     Client Service
------------------------ */
class ClientService : public Service
{
public:
      ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
      virtual ~ClientService(){}

      virtual bool Start() override;
};

/*----------------------
      Server Service
------------------------ */
class ServerService : public Service
{
public:
      ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
      virtual ~ServerService(){}

      virtual bool Start() override;
      virtual void CloseService();
private:
      ListenerRef _listner;
};