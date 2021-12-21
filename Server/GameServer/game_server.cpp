#include "game_server_pch.h"

#include "game_session/game_session.h"
#include "src/network/socket_utils.h"
#include "src/thread/thread_manager.h"
#include "src/database/db_connection_pool.h"
#include "src/network/service.h"

enum
{
	kWorkerTick = 64
};

static std::atomic<bool> exit_loop = false;

void DoWorkerJob(const ServerServiceRef& service)
{
	while (!exit_loop)
	{
		LEndTickCount = GetTickCount64() + kWorkerTick;

		// 네트워크 입출력 처리
		service->GetIocpCore()->Dispatch(10);
		
		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();

	}
}

int main()
{
	SocketUtils::Init();
	ASSERT_CRASH(DBConnectionPool::GetInstance().Connect(10, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=maplestory2;Trusted_Connection=Yes;"));

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7778),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		Service::kServerType::kGame,
		500);

	ASSERT_CRASH(service->Start());

	auto& threadManager = ThreadManager::GetInstance();
	for (int32_t i = 0; i < 6; i++)
	{
		threadManager.Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}
	DoWorkerJob(service);
	exit_loop = true;
	threadManager.Join();
	SocketUtils::Clear();
}