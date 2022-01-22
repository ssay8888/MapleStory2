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
	kPktGameServerRemovePlayer = 2005,
	kPktGameClientMovePlayer = 2006,
	kPktGameServerMovePlayer = 2007,
	kPktGameServerRespawnMonster = 2008,
	kPktGameServerMoveMonster = 2009,
	kPktGameClientTakeDamage = 2010,
	kPktGameServerTakeDamage = 2011,
	kPktGameServerUpdateStat = 2012,
	kPktGameClientAttackMonster = 2013,
	kPktGameServerAttackMonster = 2014,
	kPktGameServerMonsterStatUpdate = 2015,
	kPktGameServerKillMonster = 2016,
	kPktGameClientInventoryItemMove = 2017,
	kPktGameServerDressChange = 2018,
	kPktGameClientStatUp = 2019,
	kPktGameServerStatUp = 2020,
	kPktGameClientKeySet = 2021,
	kPktGameClientItemApply = 2022,
	kPktGameServerItemQuantityUpdate = 2023,
	kPktGameClientResurrection = 2024,
	kPktGameServerResurrection = 2025,
	kPktGameClientSpRecovery = 2026,
	kPktGameClientApplySkill = 2027,
	kPktGameClientChat = 2028,
	kPktGameServerChat = 2029,
	kPktGameClientChangeMap = 2030,
	kPktGameServerChangeMap = 2031,
};



class GameServerPacketHandler
{
public:
	// Custom Handlers
	static auto HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)->bool;
	static auto HandleGameServerLogin(PacketSessionRef& session, Protocol::GameServerLogin& pkt)->bool;
	static auto HandleGameServerLoadCharacter(PacketSessionRef& session, Protocol::GameServerLoadCharacter& pkt)->bool;
	static auto HandleGameServerRespawnPlayer(PacketSessionRef& session, Protocol::GameServerRespawnPlayer& pkt)->bool;
	static auto HandleGameServerRemovePlayer(PacketSessionRef& session, Protocol::GameServerRemovePlayer& pkt)->bool;
	static auto HandleGameServerMovePlayer(PacketSessionRef& session, Protocol::GameServerMovePlayer& pkt)->bool;
	static auto HandleGameServerRespawnMonster(PacketSessionRef& session, Protocol::GameServerRespawnMonster& pkt)->bool;
	static auto HandleGameServerMoveMonster(PacketSessionRef& session, Protocol::GameServerMoveMonster& pkt)->bool;
	static auto HandleGameServerTakeDamage(PacketSessionRef& session, Protocol::GameServerTakeDamage& pkt)->bool;
	static auto HandleGameServerUpdateStat(PacketSessionRef& session, Protocol::GameServerUpdateStat& pkt)->bool;
	static auto HandleGameServerAttackMonster(PacketSessionRef& session, Protocol::GameServerAttackMonster& pkt)->bool;
	static auto HandleGameServerMonsterStatUpdate(PacketSessionRef& session, Protocol::GameServerMonsterStatUpdate& pkt)->bool;
	static auto HandleGameServerKillMonster(PacketSessionRef& session, Protocol::GameServerKillMonster& pkt)->bool;
	static auto HandleGameServerDressChange(PacketSessionRef& session, Protocol::GameServerDressChange& pkt)->bool;
	static auto HandleGameServerStatUp(PacketSessionRef& session, Protocol::GameServerStatUp& pkt)->bool;
	static auto HandleGameServerItemQuantityUpdate(PacketSessionRef& session, Protocol::GameServerItemQuantityUpdate& pkt)->bool;
	static auto HandleGameServerResurrection(PacketSessionRef& session, Protocol::GameServerResurrection& pkt)->bool;
	static auto HandleGameServerChat(PacketSessionRef& session, Protocol::GameServerChat& pkt)->bool;
	static auto HandleGameServerChangeMap(PacketSessionRef& session, Protocol::GameServerChangeMap& pkt)->bool;
	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleGameInvalid;
		}
		_packet_handler[kPktGameServerLogin] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerLogin>(HandleGameServerLogin, session, buffer, len); };
		_packet_handler[kPktGameServerLoadCharacter] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerLoadCharacter>(HandleGameServerLoadCharacter, session, buffer, len); };
		_packet_handler[kPktGameServerRespawnPlayer] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerRespawnPlayer>(HandleGameServerRespawnPlayer, session, buffer, len); };
		_packet_handler[kPktGameServerRemovePlayer] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerRemovePlayer>(HandleGameServerRemovePlayer, session, buffer, len); };
		_packet_handler[kPktGameServerMovePlayer] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerMovePlayer>(HandleGameServerMovePlayer, session, buffer, len); };
		_packet_handler[kPktGameServerRespawnMonster] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerRespawnMonster>(HandleGameServerRespawnMonster, session, buffer, len); };
		_packet_handler[kPktGameServerMoveMonster] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerMoveMonster>(HandleGameServerMoveMonster, session, buffer, len); };
		_packet_handler[kPktGameServerTakeDamage] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerTakeDamage>(HandleGameServerTakeDamage, session, buffer, len); };
		_packet_handler[kPktGameServerUpdateStat] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerUpdateStat>(HandleGameServerUpdateStat, session, buffer, len); };
		_packet_handler[kPktGameServerAttackMonster] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerAttackMonster>(HandleGameServerAttackMonster, session, buffer, len); };
		_packet_handler[kPktGameServerMonsterStatUpdate] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerMonsterStatUpdate>(HandleGameServerMonsterStatUpdate, session, buffer, len); };
		_packet_handler[kPktGameServerKillMonster] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerKillMonster>(HandleGameServerKillMonster, session, buffer, len); };
		_packet_handler[kPktGameServerDressChange] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerDressChange>(HandleGameServerDressChange, session, buffer, len); };
		_packet_handler[kPktGameServerStatUp] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerStatUp>(HandleGameServerStatUp, session, buffer, len); };
		_packet_handler[kPktGameServerItemQuantityUpdate] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerItemQuantityUpdate>(HandleGameServerItemQuantityUpdate, session, buffer, len); };
		_packet_handler[kPktGameServerResurrection] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerResurrection>(HandleGameServerResurrection, session, buffer, len); };
		_packet_handler[kPktGameServerChat] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerChat>(HandleGameServerChat, session, buffer, len); };
		_packet_handler[kPktGameServerChangeMap] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::GameServerChangeMap>(HandleGameServerChangeMap, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32_t len)
	{
		const PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return _packet_handler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::GameClientLogin& pkt) { return MakeSendBuffer(pkt, kPktGameClientLogin); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientLoading& pkt) { return MakeSendBuffer(pkt, kPktGameClientLoading); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientMovePlayer& pkt) { return MakeSendBuffer(pkt, kPktGameClientMovePlayer); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientTakeDamage& pkt) { return MakeSendBuffer(pkt, kPktGameClientTakeDamage); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientAttackMonster& pkt) { return MakeSendBuffer(pkt, kPktGameClientAttackMonster); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientInventoryItemMove& pkt) { return MakeSendBuffer(pkt, kPktGameClientInventoryItemMove); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientStatUp& pkt) { return MakeSendBuffer(pkt, kPktGameClientStatUp); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientKeySet& pkt) { return MakeSendBuffer(pkt, kPktGameClientKeySet); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientItemApply& pkt) { return MakeSendBuffer(pkt, kPktGameClientItemApply); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientResurrection& pkt) { return MakeSendBuffer(pkt, kPktGameClientResurrection); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientSpRecovery& pkt) { return MakeSendBuffer(pkt, kPktGameClientSpRecovery); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientApplySkill& pkt) { return MakeSendBuffer(pkt, kPktGameClientApplySkill); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientChat& pkt) { return MakeSendBuffer(pkt, kPktGameClientChat); }
	static SendBufferRef MakeSendBuffer(Protocol::GameClientChangeMap& pkt) { return MakeSendBuffer(pkt, kPktGameClientChangeMap); }

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