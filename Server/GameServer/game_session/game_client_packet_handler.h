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
	kPktGameClientTakeDamage = 2009,
	kPktGameServerTakeDamage = 2010,
	kPktGameServerUpdateStat = 2011,
	kPktGameClientAttackMonster = 2012,
	kPktGameServerAttackMonster = 2013,
	kPktGameServerMonsterStatUpdate = 2014,
	kPktGameServerKillMonster = 2015,
	kPktGameClientInventoryItemMove = 2016,
	kPktGameServerDressChange = 2017,
	kPktGameClientStatUp = 2018,
	kPktGameServerStatUp = 2019,
	kPktGameClientKeySet = 2020,
	kPktGameClientItemApply = 2021,
	kPktGameServerItemQuantityUpdate = 2022,
	kPktGameClientResurrection = 2023,
	kPktGameServerResurrection = 2024,
};



class GameClientPacketHandler
{
public:
	// Custom Handlers
	static auto HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)->bool;
	static auto HandleGameClientLogin(PacketSessionRef& session, Protocol::GameClientLogin& pkt)->bool;
	static auto HandleGameClientLoading(PacketSessionRef& session, Protocol::GameClientLoading& pkt)->bool;
	static auto HandleGameClientMovePlayer(PacketSessionRef& session, Protocol::GameClientMovePlayer& pkt)->bool;
	static auto HandleGameClientTakeDamage(PacketSessionRef& session, Protocol::GameClientTakeDamage& pkt)->bool;
	static auto HandleGameClientAttackMonster(PacketSessionRef& session, Protocol::GameClientAttackMonster& pkt)->bool;
	static auto HandleGameClientInventoryItemMove(PacketSessionRef& session, Protocol::GameClientInventoryItemMove& pkt)->bool;
	static auto HandleGameClientStatUp(PacketSessionRef& session, Protocol::GameClientStatUp& pkt)->bool;
	static auto HandleGameClientKeySet(PacketSessionRef& session, Protocol::GameClientKeySet& pkt)->bool;
	static auto HandleGameClientItemApply(PacketSessionRef& session, Protocol::GameClientItemApply& pkt)->bool;
	static auto HandleGameClientResurrection(PacketSessionRef& session, Protocol::GameClientResurrection& pkt)->bool;
	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleGameInvalid;
		}
		_packet_handler[kPktGameClientLogin] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientLogin>(HandleGameClientLogin, session, buffer, len); };
		_packet_handler[kPktGameClientLoading] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientLoading>(HandleGameClientLoading, session, buffer, len); };
		_packet_handler[kPktGameClientMovePlayer] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientMovePlayer>(HandleGameClientMovePlayer, session, buffer, len); };
		_packet_handler[kPktGameClientTakeDamage] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientTakeDamage>(HandleGameClientTakeDamage, session, buffer, len); };
		_packet_handler[kPktGameClientAttackMonster] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientAttackMonster>(HandleGameClientAttackMonster, session, buffer, len); };
		_packet_handler[kPktGameClientInventoryItemMove] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientInventoryItemMove>(HandleGameClientInventoryItemMove, session, buffer, len); };
		_packet_handler[kPktGameClientStatUp] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientStatUp>(HandleGameClientStatUp, session, buffer, len); };
		_packet_handler[kPktGameClientKeySet] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientKeySet>(HandleGameClientKeySet, session, buffer, len); };
		_packet_handler[kPktGameClientItemApply] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientItemApply>(HandleGameClientItemApply, session, buffer, len); };
		_packet_handler[kPktGameClientResurrection] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameClientResurrection>(HandleGameClientResurrection, session, buffer, len); };
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
	static SendBufferRef MakeSendBuffer(Protocol::GameServerTakeDamage& pkt) { return MakeSendBuffer(pkt, kPktGameServerTakeDamage); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerUpdateStat& pkt) { return MakeSendBuffer(pkt, kPktGameServerUpdateStat); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerAttackMonster& pkt) { return MakeSendBuffer(pkt, kPktGameServerAttackMonster); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerMonsterStatUpdate& pkt) { return MakeSendBuffer(pkt, kPktGameServerMonsterStatUpdate); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerKillMonster& pkt) { return MakeSendBuffer(pkt, kPktGameServerKillMonster); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerDressChange& pkt) { return MakeSendBuffer(pkt, kPktGameServerDressChange); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerStatUp& pkt) { return MakeSendBuffer(pkt, kPktGameServerStatUp); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerItemQuantityUpdate& pkt) { return MakeSendBuffer(pkt, kPktGameServerItemQuantityUpdate); }
	static SendBufferRef MakeSendBuffer(Protocol::GameServerResurrection& pkt) { return MakeSendBuffer(pkt, kPktGameServerResurrection); }

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