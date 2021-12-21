#pragma once
#include "src/network/session.h"

class ServerSession final : public PacketSession
{
public:
	ServerSession();
	virtual ~ServerSession() = default;

	void OnConnected() override;

	void OnRecvPacket(BYTE* buffer, int32_t len) override;

	void OnSend(int32_t len) override;

	void OnDisconnected() override;

private:
	int32_t _id;
};

