#include "game_server_pch.h"
#include "game_server.h"

#include "center_client_session/center_client_session.h"
#include "center_client_session/center_login_server_packet_handler.h"
#include "data_reader/data_reader_manager.h"
#include "game/loader/loader.h"
#include "game/map/map_manager.h"
#include "game_session/game_client_packet_handler.h"
#include "game_tick/game_tick.h"
#include "src/database/db_connection_pool.h"
#include "src/network/service.h"
#include "src/network/socket_utils.h"
#include "src/system/graphic/graphic_device.h"
#include "src/thread/thread_manager.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"

std::atomic<int64_t> default_object_id = 1;

void GameServer::GameServerInit()
{
	if (SUCCEEDED(GraphicDevice::GetInstance().ReadyGraphicDevice(NULL, GraphicDevice::kWindowMode::kModeWin, 1280, 720, nullptr)))
	{
		GameLogicManager::InitDevice(NULL, NULL, 7);
		auto& componentManager = ComponentManager::GetInstance();
		auto device = GraphicDevice::GetInstance().GetDevice();
		componentManager.AddPrototype(0, TEXT("Prototype_Collider_AABB"), Collider::Create(device, Collider::kTypeAabb));
		

		componentManager.AddPrototype(0, TEXT("Prototype_Collider_OBB"), Collider::Create(device, Collider::kTypeObb));
									  
									  
		componentManager.AddPrototype(0, TEXT("Prototype_Collider_NoTarget_OBB"), Collider::Create(device, Collider::kNoTargetObb));
									  
									  
		componentManager.AddPrototype(0, TEXT("Prototype_Collider_NoTarget_AABB"), Collider::Create(device, Collider::kNoTargetAabb));
	}
	SocketUtils::Init();
	CenterLoginServerPacketHandler::Init();
	GameClientPacketHandler::Init();
	DataReaderManager::GetInstance().Init(nullptr);
	MapManager::GetInstance();
	ASSERT_CRASH(DBConnectionPool::GetInstance().Connect(10, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=maplestory2;Trusted_Connection=Yes;"));
	Loader::GetInstance();
	GameTick::GetInstance()->GameLoopInit();
	CreateServerService();
	CreateClientService();
}

void GameServer::CreateServerService()
{
	//�÷��̾ �޴� ���Ӽ��� ����
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"211.219.146.217", 7778),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager ��
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

					// ��Ʈ��ũ ����� ó��
					service->GetIocpCore()->Dispatch(10);

					// ����� �ϰ� ó��
					ThreadManager::DistributeReservedJobs();

					// �۷ι� ť
					ThreadManager::DoGlobalQueueWork();

				}
			});
	}

}

void GameServer::CreateClientService()
{
	//�α��μ����� ����Ǵ� ��������

	_center_service = MakeShared<ClientService>(
		NetAddress(L"211.219.146.217", 7779),
		MakeShared<IocpCore>(),
		MakeShared<CenterClientSession>, // TODO : SessionManager ��
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
