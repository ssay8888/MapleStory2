#pragma once
#include "protocol/protocol.pb.h"

class LoginHandler;

class LoginHandlerManager
{
private:
	LoginHandlerManager();
	~LoginHandlerManager();

public:
	static auto GetInstance()->LoginHandlerManager&
	{
		static LoginHandlerManager instance;
		return instance;
	}

	void LoginRequest(const PacketSessionRef& session, const Protocol::ClientLogin& pkt) const;

private:
	std::shared_ptr<LoginHandler> _handle = nullptr;
};

