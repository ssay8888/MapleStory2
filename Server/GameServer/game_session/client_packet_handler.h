#pragma once
#include "protocol/protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_INGAME = 1002,
	PKT_S_INGAME = 1003,
	PKT_C_SPAWNPLAYER = 1004,
	PKT_S_SPAWNPLAYER = 1005,
	PKT_C_ADDPLAYER = 1006,
	PKT_S_ADDPLAYER = 1007,
	PKT_C_MOVEPLAYER = 1008,
	PKT_S_MOVEPLAYER = 1009,
	PKT_C_REMOVEPLAYER = 1010,
	PKT_S_REMOVEPLAYER = 1011,
	PKT_C_RESPAWNMONSTER = 1012,
	PKT_S_RESPAWNMONSTER = 1013,
	PKT_C_REMOVEMONSTER = 1014,
	PKT_S_REMOVEMONSTER = 1015,
	PKT_C_MONSTERCOLLISTION = 1016,
	PKT_S_MONSTERCOLLISTION = 1017,
	PKT_C_CHANGESCALE = 1018,
	PKT_S_CHANGESCALE = 1019,
	PKT_C_USERCOLLISTION = 1020,
	PKT_S_USERCOLLISTION = 1021,
	PKT_C_CONNECTEDUSER = 1022,
	PKT_S_CONNECTEDUSER = 1023,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef session, BYTE* buffer, int32 len);
bool Handle_C_LOGIN(PacketSessionRef session, Protocol::C_LOGIN& pkt);
bool Handle_C_INGAME(PacketSessionRef session, BYTE* buffer, int32 len);
bool Handle_C_MOVEPLAYER(PacketSessionRef session, BYTE* buffer, int32 len);
bool Handle_C_MONSTERCOLLISTION(PacketSessionRef session, BYTE* buffer, int32 len);
bool Handle_C_USERCOLLISTION(PacketSessionRef session, BYTE* buffer, int32 len);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_LOGIN] = [](PacketSessionRef session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_C_INGAME] = [](PacketSessionRef session, BYTE* buffer, int32 len) { return Handle_C_INGAME(session, buffer, len); };
		GPacketHandler[PKT_C_MOVEPLAYER] = [](PacketSessionRef session, BYTE* buffer, int32 len) { return Handle_C_MOVEPLAYER(session, buffer, len); };
		GPacketHandler[PKT_C_MONSTERCOLLISTION] = [](PacketSessionRef session, BYTE* buffer, int32 len) { return Handle_C_MONSTERCOLLISTION(session, buffer, len); };
		GPacketHandler[PKT_C_USERCOLLISTION] = [](PacketSessionRef session, BYTE* buffer, int32 len) { return Handle_C_USERCOLLISTION(session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;
		
		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};