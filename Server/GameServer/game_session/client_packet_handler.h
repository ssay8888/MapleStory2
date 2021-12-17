#pragma once
#include "protocol/protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32_t)>;

enum : uint16_t
{
	kPktClientLogin = 1000,
	kPktServerLogin = 1001,
	kPktClientCreateCharacter = 1002,
	kPktServerCreateCharacter = 1003,
};

// Custom Handlers
bool HandleInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len);
bool HandleClientLogin(PacketSessionRef& session, Protocol::ClientLogin& pkt);
bool HandleClientCreateCharacter(PacketSessionRef& session, Protocol::ClientCreateCharacter& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleInvalid;
		}
		_packet_handler[kPktClientLogin] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::ClientLogin>(HandleClientLogin, session, buffer, len); };
		_packet_handler[kPktClientCreateCharacter] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::ClientCreateCharacter>(HandleClientCreateCharacter, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32_t len)
	{
		const PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return _packet_handler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::ServerLogin& pkt) { return MakeSendBuffer(pkt, kPktServerLogin); }
	static SendBufferRef MakeSendBuffer(Protocol::ServerCreateCharacter& pkt) { return MakeSendBuffer(pkt, kPktServerCreateCharacter); }

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