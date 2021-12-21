#include "game_server_pch.h"

#include "game_server.h"
#include "src/network/socket_utils.h"
#include "src/thread/thread_manager.h"

int main()
{
	GameServer gameServer;

	gameServer.GameServerInit();
	auto& threadManager = ThreadManager::GetInstance();
	threadManager.Join();
	SocketUtils::Clear();
}