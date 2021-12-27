#include "c_pch.h"
#include "game_init.h"

#include "protocol/game_protocol.pb.h"
#include "src/main/main_app.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/service.h"

GameInit::GameInit(const ComPtr<IDirect3DDevice9>& device):
	Scene(device)
{
}
auto GameInit::NativeConstruct() -> HRESULT
{
	return Scene::NativeConstruct();
}

auto GameInit::Tick(double timeDelta) -> int32_t
{
	//��ƽ�� ���� ������ �ε��� ������.
	//���Ӽ����� auth�� �����Ѵ�.
	//���Ӽ��� auth�� ��ϵ� ������ ���� ĳ���͸� �ҷ��;���.


	if (!_is_first_send)
	{
		_is_first_send = true;
		Protocol::GameClientLogin sendPkt;
		auto authInfo = g_mainApp->GetAuthInfo();
		sendPkt.set_auth(authInfo.auth());
		g_service->Broadcast(GameServerPacketHandler::MakeSendBuffer(sendPkt));
	}
	EnableWindow(g_hEdit, false);

	return Scene::Tick(timeDelta);
}

auto GameInit::Render() -> HRESULT
{
	return Scene::Render();
}

auto GameInit::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<GameInit>
{
	auto instance = std::make_shared<GameInit>(device);

	if (FAILED(instance->NativeConstruct()))
	{
		MSGBOX("Failed to Create GameInit");
		return nullptr;
	}

	return instance;
}
