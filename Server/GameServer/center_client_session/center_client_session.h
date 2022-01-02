#pragma once

class CenterClientSession final : public PacketSession
{
public:
	CenterClientSession();
	virtual ~CenterClientSession()
	{
		std::cout << "~CenterClientSession" << std::endl;
	}

public:
	auto OnConnected() -> void override;
	auto OnDisconnected() -> void override;
	auto OnRecvPacket(BYTE* buffer, int32_t len) -> void override;
	auto OnSend(int32_t len) -> void override;

private:
	auto ConnectedCenterServerInit()->void;

public:
	auto GetSessionId() const->int64_t;
	
private:
	int64_t										_session_id = -1;
};
