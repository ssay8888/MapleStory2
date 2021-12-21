#pragma once
#include "protocol/center_login_protocol.pb.h"

class CenterHandler;

class CenterHandlerManager
{
private:
	CenterHandlerManager();
	~CenterHandlerManager() = default;

public:
	static auto GetInstance()->CenterHandlerManager&
	{
		static CenterHandlerManager instance;
		return instance;
	}
	void SocketConnectInit(PacketSessionRef& session, Protocol::CenterLoginClientInit& pkt) const;
	
private:
	std::shared_ptr<CenterHandler> _handle = nullptr;
};

