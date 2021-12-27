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
	kPktLoginClientCharacterSelect = 1006,
	kPktLoginServerCharacterSelect = 1007,
};

class LoginClientPacketHandler
{
public:
// Custom Handlers
	static auto HandleLoginInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)->bool;
	static auto HandleLoginClientLogin(const PacketSessionRef& session, const Protocol::LoginClientLogin& pkt)->bool;
	static auto HandleLoginClientCharacterList(const PacketSessionRef& session, const Protocol::LoginClientCharacterList& pkt)->bool;
	static auto HandleLoginClientCreateCharacter(const PacketSessionRef& session, const Protocol::LoginClientCreateCharacter& pkt)->bool;
	static auto HandleLoginClientCharacterSelect(const PacketSessionRef& session, const Protocol::LoginClientCharacterSelect& pkt)->bool;

	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleLoginInvalid;
		}
		_packet_handler[kPktLoginClientLogin] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::LoginClientLogin>(HandleLoginClientLogin, session, buffer, len); };
		_packet_handler[kPktLoginClientCharacterList] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::LoginClientCharacterList>(HandleLoginClientCharacterList, session, buffer, len); };
		_packet_handler[kPktLoginClientCreateCharacter] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::LoginClientCreateCharacter>(HandleLoginClientCreateCharacter, session, buffer, len); };
		_packet_handler[kPktLoginClientCharacterSelect] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::LoginClientCharacterSelect>(HandleLoginClientCharacterSelect, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32_t len)
	{
		const PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return _packet_handler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::LoginServerLogin& pkt) { return MakeSendBuffer(pkt, kPktLoginServerLogin); }
	static SendBufferRef MakeSendBuffer(Protocol::LoginServerCharacterList& pkt) { return MakeSendBuffer(pkt, kPktLoginServerCharacterList); }
	static SendBufferRef MakeSendBuffer(Protocol::LoginServerCreateCharacter& pkt) { return MakeSendBuffer(pkt, kPktLoginServerCreateCharacter); }
	static SendBufferRef MakeSendBuffer(Protocol::LoginServerCharacterSelect& pkt) { return MakeSendBuffer(pkt, kPktLoginServerCharacterSelect); }

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