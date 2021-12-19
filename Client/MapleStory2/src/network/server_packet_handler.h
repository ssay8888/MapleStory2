#pragma once
#include "protocol/protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32_t)>;

enum : uint16_t
{
	kPktClientLogin = 1000,
	kPktServerLogin = 1001,
	kPktClientCharacterList = 1002,
	kPktServerCharacterList = 1003,
	kPktClientCreateCharacter = 1004,
	kPktServerCreateCharacter = 1005,
};

// Custom Handlers
bool HandleInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len);
bool HandleServerLogin(PacketSessionRef& session, Protocol::ServerLogin& pkt);
bool HandleServerCharacterList(PacketSessionRef& session, Protocol::ServerCharacterList& pkt);
bool HandleServerCreateCharacter(PacketSessionRef& session, Protocol::ServerCreateCharacter& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleInvalid;
		}
		_packet_handler[kPktServerLogin] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::ServerLogin>(HandleServerLogin, session, buffer, len); };
		_packet_handler[kPktServerCharacterList] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::ServerCharacterList>(HandleServerCharacterList, session, buffer, len); };
		_packet_handler[kPktServerCreateCharacter] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::ServerCreateCharacter>(HandleServerCreateCharacter, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32_t len)
	{
		const PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return _packet_handler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::ClientLogin& pkt) { return MakeSendBuffer(pkt, kPktClientLogin); }
	static SendBufferRef MakeSendBuffer(Protocol::ClientCharacterList& pkt) { return MakeSendBuffer(pkt, kPktClientCharacterList); }
	static SendBufferRef MakeSendBuffer(Protocol::ClientCreateCharacter& pkt) { return MakeSendBuffer(pkt, kPktClientCreateCharacter); }

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