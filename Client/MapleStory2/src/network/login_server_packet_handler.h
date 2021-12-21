#pragma once
#include "protocol/login_protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32_t)>;

enum : uint16_t
{
	kPktLoginClientLogin = 1000,
	kPktLoginServerLogin = 1001,
	kPktLoginClientCharacterList = 1002,
	kPktLoginServerCharacterList = 1003,
	kPktLoginClientCreateCharacter = 1004,
	kPktLoginServerCreateCharacter = 1005,
};

class LoginServerPacketHandler
{
public:
// Custom Handlers
	static auto HandleLoginInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)->bool;
	static auto HandleLoginServerLogin(PacketSessionRef& session, Protocol::LoginServerLogin& pkt)->bool;
	static auto HandleLoginServerCharacterList(PacketSessionRef& session, Protocol::LoginServerCharacterList& pkt)->bool;
	static auto HandleLoginServerCreateCharacter(PacketSessionRef& session, Protocol::LoginServerCreateCharacter& pkt)->bool;

	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleLoginInvalid;
		}
		_packet_handler[kPktLoginServerLogin] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::LoginServerLogin>(HandleLoginServerLogin, session, buffer, len); };
		_packet_handler[kPktLoginServerCharacterList] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::LoginServerCharacterList>(HandleLoginServerCharacterList, session, buffer, len); };
		_packet_handler[kPktLoginServerCreateCharacter] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::LoginServerCreateCharacter>(HandleLoginServerCreateCharacter, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32_t len)
	{
		const PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return _packet_handler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::LoginClientLogin& pkt) { return MakeSendBuffer(pkt, kPktLoginClientLogin); }
	static SendBufferRef MakeSendBuffer(Protocol::LoginClientCharacterList& pkt) { return MakeSendBuffer(pkt, kPktLoginClientCharacterList); }
	static SendBufferRef MakeSendBuffer(Protocol::LoginClientCreateCharacter& pkt) { return MakeSendBuffer(pkt, kPktLoginClientCreateCharacter); }

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