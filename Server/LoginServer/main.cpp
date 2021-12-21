#include "pch.h"

#include "login_server.h"
#include "src/thread/thread_manager.h"
#include "src/network/socket_utils.h"

std::shared_ptr<LoginServer> g_login_server;

int main()
{
	g_login_server = std::make_shared<LoginServer>();

	g_login_server->LoginServerInit();

	auto& threadManager = ThreadManager::GetInstance();
	threadManager.Join();
	SocketUtils::Clear();

}
