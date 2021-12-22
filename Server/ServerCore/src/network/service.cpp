#include "pch.h"
#include "service.h"
#include "session.h"
#include "listener.h"

/*-------------
	Service
--------------*/

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, kServerType serverType, int32_t maxSessionCount) :
	_type(type),
	_netAddress(address),
	_iocp_core(core),
	_session_factory(factory),
	_server_type(serverType),
	_max_session_count(maxSessionCount)
{
}

auto Service::CanStart() const -> bool
{
	return _session_factory != nullptr;
}

auto Service::CloseService() -> void
{
	WRITE_LOCK;
	for (const auto& session : _sessions)
	{
		session->Disconnect(L"");
	}
}

auto Service::SetSessionFactory(const SessionFactory func) -> void
{
	_session_factory = func;
}

auto Service::Broadcast(const SendBufferRef sendBuffer) -> void
{
	WRITE_LOCK;
	for (const auto& session : _sessions)
	{
		session->Send(sendBuffer);
	}
}

auto Service::CreateSession() -> SessionRef
{
	SessionRef session = _session_factory();
	session->SetService(shared_from_this());

	if (_iocp_core->Register(session) == false)
		return nullptr;

	return session;
}

auto Service::AddSession(const SessionRef session) -> void
{
	WRITE_LOCK;
	_session_count++;
	_sessions.insert(session);
}

auto Service::ReleaseSession(const SessionRef session) -> void
{
	WRITE_LOCK;
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_session_count--;
}

auto Service::GetCurrentSessionCount() const -> int32_t
{
	return _session_count;
}

auto Service::GetMaxSessionCount() const -> int32_t
{
	return _max_session_count;
}

auto Service::GetServiceType() const -> ServiceType
{
	return _type;
}

auto Service::GetNetAddress() const -> NetAddress
{
	return _netAddress;
}

auto Service::GetIocpCore() -> IocpCoreRef&
{
	return _iocp_core;
}

auto Service::GetServerType() const -> const kServerType&
{
	return _server_type;
}

/*-----------------
	ClientService
------------------*/

ClientService::ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, kServerType serverType, int32_t maxSessionCount)
	: Service(ServiceType::Client, targetAddress, core, factory, serverType, maxSessionCount)
{
}

auto ClientService::Start() -> bool
{
	if (CanStart() == false)
		return false;

	const int32_t sessionCount = GetMaxSessionCount();
	for (int32_t i = 0; i < sessionCount; i++)
	{
		const SessionRef session = CreateSession();
		if (session && session->Connect() == false)
			return false;
	}

	return true;
}

ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, kServerType serverType, int32_t maxSessionCount)
	: Service(ServiceType::Server, address, core, factory, serverType, maxSessionCount)
{
}

auto ServerService::Start() -> bool
{
	if (CanStart() == false)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServerServiceRef service = std::static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

auto ServerService::CloseService() -> void
{
	Service::CloseService();
}
