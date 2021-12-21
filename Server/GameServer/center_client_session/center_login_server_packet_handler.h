#pragma once
#include "protocol/center_login_protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32_t)>;

enum : uint16_t
{
	kPktCenterLoginClientInit = 1000,
	kPktCenterLoginServerInit = 1001,
	kPktCenterLoginClientCreateAuth = 1002,
	kPktCenterLoginServerCreateAuth = 1003,
	kPktCenterLoginClientCheckAuth = 1004,
	kPktCenterLoginServerCheckAuth = 1005,
};

class CenterLoginServerPacketHandler
{
public:
// Custom Handlers
	static auto HandleLoginInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)->bool;
	static auto HandleCenterLoginServerInit(PacketSessionRef& session, Protocol::CenterLoginServerInit& pkt)->bool;
	static auto HandleCenterLoginServerCreateAuth(PacketSessionRef& session, Protocol::CenterLoginServerCreateAuth& pkt)->bool;
	static auto HandleCenterLoginServerCheckAuth(PacketSessionRef& session, Protocol::CenterLoginServerCheckAuth& pkt)->bool;

	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleLoginInvalid;
		}
		_packet_handler[kPktCenterLoginServerInit] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::CenterLoginServerInit>(HandleCenterLoginServerInit, session, buffer, len); };
		_packet_handler[kPktCenterLoginServerCreateAuth] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::CenterLoginServerCreateAuth>(HandleCenterLoginServerCreateAuth, session, buffer, len); };
		_packet_handler[kPktCenterLoginServerCheckAuth] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::CenterLoginServerCheckAuth>(HandleCenterLoginServerCheckAuth, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32_t len)
	{
		const PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return _packet_handler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::CenterLoginClientInit& pkt) { return MakeSendBuffer(pkt, kPktCenterLoginClientInit); }
	static SendBufferRef MakeSendBuffer(Protocol::CenterLoginClientCreateAuth& pkt) { return MakeSendBuffer(pkt, kPktCenterLoginClientCreateAuth); }
	static SendBufferRef MakeSendBuffer(Protocol::CenterLoginClientCheckAuth& pkt) { return MakeSendBuffer(pkt, kPktCenterLoginClientCheckAuth); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, const int32_t len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, const uint16_t pktId)
	{
		const auto dataSize = static_cast<uint16_t>(pkt.ByteSizeLong());
		const uint16_t packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(packetSize);
		auto header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}

private:
	static PacketHandlerFunc _packet_handler[UINT16_MAX];
};