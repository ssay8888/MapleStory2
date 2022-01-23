#include "c_pch.h"
#include "scene_logo.h"

#include <codecvt>
#include <iostream>
#include <locale>

#include "src/game_object/back_ground/back_ground.h"
#include "src/game_object/ui/login/text_box_ui.h"
#include "src/managers/sound_manager/sound_manager.h"
#include "src/network/send_manager.h"
#include "src/network/login_server_packet_handler.h"
#include "src/scene/loading/scene_loading.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/scene_utility/scene_manager.h"
#include "src/utility/timers/timer_manager.h"
#include "src/utils/file_utils.h"

SceneLogo::SceneLogo(const ComPtr<IDirect3DDevice9>& device) :
	Scene(device)
{
}

SceneLogo::~SceneLogo()
{

}

auto SceneLogo::NativeConstruct() -> HRESULT
{
	Scene::NativeConstruct();

	if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(ReadyLayerTextBox(TEXT("Layer_LoginBox"))))
		return E_FAIL;
	if (FAILED(ReadyMouse()))
		return E_FAIL;

	auto& timerManager = TimerManager::GetInstance();
	timerManager.AddTimers(TEXT("LoginTryTimer"));
	SoundManager::GetInstance().PlayBGM(L"BGM_Henesys_01.wav");
	return S_OK;
}

auto SceneLogo::Tick(const double timeDelta) -> int32_t
{
	Scene::Tick(timeDelta);

	if (InputDevice::GetInstance().GetKeyDown(DIK_RETURN))
	{
		const auto& objectManager = ObjectManager::GetInstance();
		auto& timerManager = TimerManager::GetInstance();

		if (timerManager.IsTimeCheck(TEXT("LoginTryTimer"), 1))
		{
			timerManager.ResetTime(TEXT("LoginTryTimer"));
			Protocol::LoginClientLogin pkt;
			const auto id = std::static_pointer_cast<TextBoxUi>(objectManager.GetGameObjectPtr(kScene::kSceneLogo,
			                                                                                   TEXT("Layer_LoginBox"),
			                                                                                   0));
			const auto pw = std::static_pointer_cast<TextBoxUi>(objectManager.GetGameObjectPtr(kScene::kSceneLogo,
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
	}
	return S_OK;
}

auto SceneLogo::Render() -> HRESULT
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
	const auto& componentManager = ComponentManager::GetInstance();
	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Login_TextBox"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/login_ui/textbox_%d.png"), 2))))
	{
		return E_FAIL;
	}
	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Login_Btn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/login_ui/login_btn_%d.png"), 2))))
	{
		return E_FAIL;
	}

	TagTextBoxInformation info;
	info.size = { 263.f, 30.f };
	info.pos = { g_WinCX >> 1, g_WinCY >> 1 };

	if (FAILED(ObjectManager::GetInstance().AddGameObject(static_cast<int32_t>(kScene::kSceneLogo), TEXT("Prototype_Login_Textbox"), layerTag, &info)))
	{
		return E_FAIL;
	}
	info.pos = { g_WinCX >> 1, (g_WinCY >> 1) + 30 };
	info.is_password = true;
	if (FAILED(ObjectManager::GetInstance().AddGameObject(static_cast<int32_t>(kScene::kSceneLogo), TEXT("Prototype_Login_Textbox"), layerTag, &info)))
	{
		return E_FAIL;
	}

	return S_OK;
}

auto SceneLogo::ReadyMouse() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();
	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Click_Mouse"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/mouse/click_%d.png"), 3))))
	{
		return E_FAIL;
	}
	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Grab_Mouse"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/mouse/grab_%d.png"), 3))))
	{
		return E_FAIL;
	}


	if (FAILED(ObjectManager::GetInstance().AddGameObject(static_cast<int32_t>(kScene::kSceneStatic), TEXT("Prototype_Mouse"), L"Layer_Mouse")))
	{
		return E_FAIL;
	}
	return S_OK;
}
