#include "pch.h"
#include "login_server.h"

#include "center_server_session/center_login_client_packet_handler.h"
#include "login_session/login_client_packet_handler.h"
#include "login_session/login_session.h"
#include "src/network/socket_utils.h"
#include "src/database/db_connection_pool.h"
#include "src/network/service.h"
#include "src/thread/thread_manager.h"

auto LoginServer::LoginServerInit() -> void
{
	SocketUtils::Init();
	LoginClientPacketHandler::Init();
	CenterLoginClientPacketHandler::Init();
	ASSERT_CRASH(DBConnectionPool::GetInstance().Connect(10, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=maplestory2;Trusted_Connection=Yes;"));
	LoginServerServiceCreate();
	LoginCenterServerServiceCreate();
}

auto LoginServer::LoginServerServiceCreate() -> void
{
	//로그인하는 유저를 받는서버 형태
	_login_service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<LoginSession>, // TODO : SessionManager 등
		Service::kServerType::kServerLogin,
		500);

	ASSERT_CRASH(_login_service->Start());

	auto& threadManager = ThreadManager::GetInstance();
	for (int32_t i = 0; i < 6; i++)
	{
		threadManager.Launch(
			[this]()
			{
				while (!_exit_loop)
				{
					LEndTickCount = GetTickCount64() + kWorkerTick;

					_login_service->GetIocpCore()->Dispatch(10);

					ThreadManager::DistributeReservedJobs();

					ThreadManager::DoGlobalQueueWork();
				}
			});
	}
}

auto LoginServer::LoginCenterServerServiceCreate() -> void
{

	//게임서버와의 통신서버
	_center_service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7779),
		MakeShared<IocpCore>(),
		MakeShared<LoginSession>, // TODO : SessionManager 등
		Service::kServerType::kCenterServerLogin);

	ASSERT_CRASH(_center_service->Start());
	auto& threadManager = ThreadManager::GetInstance();

	for (int32_t i = 0; i < 2; i++)
	{
		threadManager.Launch(
			[this]()
			{
				while (!_exit_loop)
				{
					LEndTickCount = GetTickCount64() + kWorkerTick;

					_center_service->GetIocpCore()->Dispatch(10);

					ThreadManager::DistributeReservedJobs();

					ThreadManager::DoGlobalQueueWork();
				}
			});
	}


}

auto LoginServer::GetCenterServerSevice() const -> ServerServiceRef
{
	return _center_service;
}

auto LoginServer::GetLoginServerSevice() const -> ServerServiceRef
{
	return _login_service;
}
