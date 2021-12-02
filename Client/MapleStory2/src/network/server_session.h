#pragma once
#include "src/network/session.h"

class ServerSession final : public PacketSession
{
public:
	ServerSession();
	~ServerSession() = default;

	virtual void OnConnected() override;

	virtual void OnRecvPacket(BYTE* buffer, int32_t len) override;

	virtual void OnSend(int32_t len) override;

	virtual void OnDisconnected() override;

private:
	int32_t _id;
};

