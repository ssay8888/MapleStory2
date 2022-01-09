#pragma once
#include "protocol/game_protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32_t)>;

enum : uint16_t
{
	kPktGameClientLogin = 2000,
	kPktGameServerLogin = 2001,
	kPktGameServerLoadCharacter = 2002,
	kPktGameClientLoading = 2003,
	kPktGameServerRespawnPlayer = 2004,
	kPktGameClientMovePlayer = 2005,
	kPktGameServerMovePlayer = 2006,
	kPktGameServerRespawnMonster = 2007,
	kPktGameServerMoveMonster = 2008,
};



class GameClientPacketHandler
{
public:
	// Custom Handlers
	static auto HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)->bool;
	static auto HandleGameClientLogin(PacketSessionRef& session, Protocol::GameClientLogin& pkt)->bool;
	static auto HandleGameClientLoading(PacketSessionRef& session, Protocol::GameClientLoading& pkt)->bool;
	static auto HandleGameClientMovePlayer(PacketSessionRef& session, Protocol::GameClientMovePlayer& pkt)->bool;
	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleGameInvalid;
		}
		_packet_handler[kPktGameClientLogin] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientLogin>(HandleGameClientLogin, session, buffer, len); };
		_packet_handler[kPktGameClientLoading] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientLoading>(HandleGameClientLoading, session, buffer, len); };
		_packet_handler[kPktGameClientMovePlayer] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientMovePlayer>(HandleGameClientMovePlayer, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32_t len)
	{
		const PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return _packet_handler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::GameServerLogin& pkt) { return MakeSendBuffer(pkt, kPktGameServerLogin); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerLoadCharacter& pkt) { return MakeSendBuffer(pkt, kPktGameServerLoadCharacter); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerRespawnPlayer& pkt) { return MakeSendBuffer(pkt, kPktGameServerRespawnPlayer); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerMovePlayer& pkt) { return MakeSendBuffer(pkt, kPktGameServerMovePlayer); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerRespawnMonster& pkt) { return MakeSendBuffer(pkt, kPktGameServerRespawnMonster); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerMoveMonster& pkt) { return MakeSendBuffer(pkt, kPktGameServerMoveMonster); }

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