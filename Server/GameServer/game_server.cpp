#include "game_server_pch.h"
#include "game_server.h"

#include "center_client_session/center_client_session.h"
#include "center_client_session/center_login_server_packet_handler.h"
#include "game/loader/loader.h"
#include "game_session/game_client_packet_handler.h"
#include "src/database/db_connection_pool.h"
#include "src/network/service.h"
#include "src/network/socket_utils.h"
#include "src/thread/thread_manager.h"

void GameServer::GameServerInit()
{
	SocketUtils::Init();
	CenterLoginServerPacketHandler::Init();
	GameClientPacketHandler::Init();
	ASSERT_CRASH(DBConnectionPool::GetInstance().Connect(10, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=maplestory2;Trusted_Connection=Yes;"));
	Loader::GetInstance();
	CreateServerService();
	CreateClientService();
}

void GameServer::CreateServerService()
{
	//플레이어를 받는 게임서버 형태
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7778),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		Service::kServerType::kServerGame,
		500);

	ASSERT_CRASH(service->Start());

	auto& threadManager = ThreadManager::GetInstance();
	for (int32_t i = 0; i < 6; i++)
	{
		threadManager.Launch([service, this]()
			{
				while (!_exit_loop)
				{
					LEndTickCount = GetTickCount64() + kWorkerTick;

					// 네트워크 입출력 처리
					service->GetIocpCore()->Dispatch(10);

					// 예약된 일감 처리
					ThreadManager::DistributeReservedJobs();

					// 글로벌 큐
					ThreadManager::DoGlobalQueueWork();

				}
			});
	}

}

void GameServer::CreateClientService()
{
	//로그인서버로 연결되는 서비스형태

	_center_service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7779),
		MakeShared<IocpCore>(),
		MakeShared<CenterClientSession>, // TODO : SessionManager 등
		Service::kServerType::kCenterClientGame,
		1);

	ASSERT_CRASH(_center_service->Start());

	auto& threadManager = ThreadManager::GetInstance();
	for (int32_t i = 0; i < 2; i++)
	{
		threadManager.Launch([this]()
		{
			while (!_exit_loop)
			{
				LEndTickCount = GetTickCount64() + kWorkerTick;

				_center_service->GetIocpCore()->Dispatch(10);
				
			}
		});
	}

}
