#pragma once
class GameServer
{
public:
	enum
	{
		kWorkerTick = 64
	};

	GameServer() = default;

	void GameServerInit();
private:
	void CreateServerService();
	void CreateClientService();

private:
	std::atomic<bool> _exit_loop = false;
	ClientServiceRef _center_service;
};

