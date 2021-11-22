#pragma once
#include "net_address.h"
#include "iocp_core.h"
#include "listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client
};

/*-------------
	Service
--------------*/

using SessionFactory = std::function<SessionRef(void)>;

class Service : public std::enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service() = default;

	virtual auto Start() -> bool abstract;
	auto CanStart() const -> bool ;

	virtual auto CloseService() -> void;
	auto SetSessionFactory(SessionFactory func) -> void ;

	auto Broadcast(SendBufferRef sendBuffer) -> void;
	auto CreateSession() -> SessionRef;
	auto AddSession(SessionRef session) -> void;
	auto ReleaseSession(SessionRef session) -> void;
	auto GetCurrentSessionCount() const -> int32 ;
	auto GetMaxSessionCount() const -> int32 ;
		 
public:	 
	auto GetServiceType() const -> ServiceType ;
	auto GetNetAddress() const -> NetAddress ;
	auto GetIocpCore() -> IocpCoreRef& ;

protected:
	USE_LOCK;
	ServiceType			_type;
	NetAddress			_netAddress = {};
	IocpCoreRef			_iocp_core;

	Set<SessionRef>		_sessions;
	int32				_session_count = 0;
	int32				_max_session_count = 0;
	SessionFactory		_session_factory;
};

/*-----------------
	ClientService
------------------*/

class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientService()
	{
	}
	

	virtual auto Start() -> bool override;
};


/*-----------------
	ServerService
------------------*/

class ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerService() = default;

	virtual auto Start() -> bool override;
	virtual auto CloseService() -> void override;

private:
	ListenerRef		_listener = nullptr;
};