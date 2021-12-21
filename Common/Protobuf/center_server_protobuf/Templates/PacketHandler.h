#pragma once
#include "protocol/center_login_protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32_t)>;

enum : uint16_t
{
{%- for pkt in parser.total_pkt %}
	kPkt{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

class {{output}}
{
public:
// Custom Handlers
	static auto HandleLoginInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)->bool;

	{%- for pkt in parser.recv_pkt %}
	static auto Handle{{pkt.name}}(PacketSessionRef& session, Protocol::{{pkt.name}}& pkt)->bool;
	{%- endfor %}

	static void Init()
	{
		for (auto& handler : _packet_handler)
		{
			handler = HandleLoginInvalid;
		}

{%- for pkt in parser.recv_pkt %}
		_packet_handler[kPkt{{pkt.name}}] = [](PacketSessionRef& session, BYTE* buffer, int32_t len) { return HandlePacket<Protocol::{{pkt.name}}>(Handle{{pkt.name}}, session, buffer, len); };
{%- endfor %}
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32_t len)
	{
		const PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return _packet_handler[header->id](session, buffer, len);
	}

{%- for pkt in parser.send_pkt %}
	static SendBufferRef MakeSendBuffer(Protocol::{{pkt.name}}& pkt) { return MakeSendBuffer(pkt, kPkt{{pkt.name}}); }
{%- endfor %}

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
