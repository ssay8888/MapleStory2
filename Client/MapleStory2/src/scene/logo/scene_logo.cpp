#include "c_pch.h"
#include "scene_logo.h"

#include <codecvt>
#include <iostream>
#include <locale>

#include "src/game_object/back_ground/back_ground.h"
#include "src/game_object/ui/login/text_box_ui.h"
#include "src/network/send_manager.h"
#include "src/network/login_server_packet_handler.h"
#include "src/scene/loading/scene_loading.h"
#include "src/system/input/input_device.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/scene_utility/scene_manager.h"
#include "src/utils/file_utils.h"

SceneLogo::SceneLogo(const ComPtr<IDirect3DDevice9>& device) :
	Scene(device)
{
}

SceneLogo::~SceneLogo()
{
}

HRESULT SceneLogo::NativeConstruct()
{
	Scene::NativeConstruct();

	if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(ReadyLayerTextBox(TEXT("Layer_LoginBox"))))
		return E_FAIL;

	return S_OK;
}

int32_t SceneLogo::Tick(const double timeDelta)
{
	Scene::Tick(timeDelta);

	if (InputDevice::GetInstance().GetKeyDown(DIK_RETURN))
	{
		//const auto scene = SceneLoading::Create(_graphic_device, kSceneGamePlay0);
		//if (SUCCEEDED(SceneManager::GetInstance().SetUpScene(scene)))
		//{
		//	GameLogicManager::Clear(static_cast<uint32_t>(kSceneLogo));
		//}

		auto& objectManager = ObjectManager::GetInstance();
		Protocol::LoginClientLogin pkt;
		auto id = std::static_pointer_cast<TextBoxUi>(objectManager.GetGameObjectPtr(kScene::kSceneLogo,
			TEXT("Layer_LoginBox"),
			0));
		auto pw = std::static_pointer_cast<TextBoxUi>(objectManager.GetGameObjectPtr(kScene::kSceneLogo,
			TEXT("Layer_LoginBox"),
			1));
		if (id && pw)
		{
			pkt.set_id(FileUtils::ConvertWtoC(id->GetText().c_str()).c_str());
			pkt.set_pw(FileUtils::ConvertWtoC(pw->GetText().c_str()).c_str());
			pkt.set_auth("");
		}

		const auto send_packet = LoginServerPacketHandler::MakeSendBuffer(pkt);
		SendManager::GetInstance().Push(send_packet);

		EnableWindow(g_hEdit, false);
	}
	return S_OK;
}

HRESULT SceneLogo::Render()
{
	return Scene::Render();
}

auto SceneLogo::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<SceneLogo>
{
	auto instance = std::make_shared<SceneLogo>(device);

	if (FAILED(instance->NativeConstruct()))
	{
		MSGBOX("Failed to Create SceneLogo");
		return nullptr;
	}

	return instance;
}

auto SceneLogo::ReadyLayerBackGround(const std::wstring& layerTag) -> HRESULT
{
	BackGround::BackGroundInfo info;
	info.prototype_texture_name = TEXT("Prototype_Texture_Login_Logo");
	info.image_size_x = 1280;
	info.image_size_y = 720;
	info.pos_x = 1280 >> 1;
	info.pos_y = 720 >> 1;
	if (FAILED(ObjectManager::GetInstance().AddGameObject(static_cast<int32_t>(kScene::kSceneLogo), TEXT("Prototype_BackGround"), layerTag, &info)))
		return E_FAIL;

	return S_OK;
}

auto SceneLogo::ReadyLayerTextBox(const std::wstring& layerTag) -> HRESULT
{
	TagTextBoxInformation info;
	info.pos = { 250, 250 };
	info.size = { 300, 36 };

	if (FAILED(ObjectManager::GetInstance().AddGameObject(static_cast<int32_t>(kScene::kSceneLogo), TEXT("Prototype_Login_Textbox"), layerTag, &info)))
		return E_FAIL;
	info.pos = { 250, 300 };
	info.is_password = true;
	if (FAILED(ObjectManager::GetInstance().AddGameObject(static_cast<int32_t>(kScene::kSceneLogo), TEXT("Prototype_Login_Textbox"), layerTag, &info)))
		return E_FAIL;

	return S_OK;
}
