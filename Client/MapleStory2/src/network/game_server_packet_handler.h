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



class GameServerPacketHandler
{
public:
	// Custom Handlers
	static auto HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)->bool;
	static auto HandleGameServerLogin(PacketSessionRef& session, Protocol::GameServerLogin& pkt)->bool;
	static auto HandleGameServerLoadCharacter(PacketSessionRef& session, Protocol::GameServerLoadCharacter& pkt)->bool;
	static auto HandleGameServerRespawnPlayer(PacketSessionRef& session, Protocol::GameServerRespawnPlayer& pkt)->bool;
	static auto HandleGameServerMovePlayer(PacketSessionRef& session, Protocol::GameServerMovePlayer& pkt)->bool;
	static auto HandleGameServerRespawnMonster(PacketSessionRef& session, Protocol::GameServerRespawnMonster& pkt)->bool;
	static auto HandleGameServerMoveMonster(PacketSessionRef& session, Protocol::GameServerMoveMonster& pkt)->bool;
	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleGameInvalid;
		}
		_packet_handler[kPktGameServerLogin] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerLogin>(HandleGameServerLogin, session, buffer, len); };
		_packet_handler[kPktGameServerLoadCharacter] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerLoadCharacter>(HandleGameServerLoadCharacter, session, buffer, len); };
		_packet_handler[kPktGameServerRespawnPlayer] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerRespawnPlayer>(HandleGameServerRespawnPlayer, session, buffer, len); };
		_packet_handler[kPktGameServerMovePlayer] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerMovePlayer>(HandleGameServerMovePlayer, session, buffer, len); };
		_packet_handler[kPktGameServerRespawnMonster] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerRespawnMonster>(HandleGameServerRespawnMonster, session, buffer, len); };
		_packet_handler[kPktGameServerMoveMonster] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerMoveMonster>(HandleGameServerMoveMonster, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32_t len)
	{
		const PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return _packet_handler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::GameClientLogin& pkt) { return MakeSendBuffer(pkt, kPktGameClientLogin); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientLoading& pkt) { return MakeSendBuffer(pkt, kPktGameClientLoading); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientMovePlayer& pkt) { return MakeSendBuffer(pkt, kPktGameClientMovePlayer); }

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