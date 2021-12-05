// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>

#include "types.h"
#include "game_session/client_packet_handler.h"
#include "game_session/game_session.h"
#include "src/memory/memory.h"
#include "src/network/net_address.h"
#include "src/thread/thread_manager.h"
#include "src/network/service.h"
#include "src/network/socket_utils.h"
#include "src/database/db_connection_pool.h"
#include "src/database/db_bind.h"
#include "src/database/db_bind_helper.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리
		service->GetIocpCore()->Dispatch(10);

		//자신이 받은 패킷 처리
	/*	while (LRecvBuffers.empty() == false)
		{
			auto buffer = LRecvBuffers.front();
			LRecvBuffers.pop();

			buffer.first->GetRecvBuffer().Clean(); 
			auto header = *reinterpret_cast<PacketHeader*>(buffer.second);
			ClientPacketHandler::HandlePacket(buffer.first, buffer.second, header.size);
			buffer.first->GetRecvBuffer().OnRead(header.size);
		}*/

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();

	}
}

int main()
{
	SocketUtils::Init();
	ClientPacketHandler::Init();
	ASSERT_CRASH(DBConnectionPool::GetInstance().Connect(1, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=maplestory2;Trusted_Connection=Yes;"));
	
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
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

	threadManager.Join();
	SocketUtils::Clear();

}
