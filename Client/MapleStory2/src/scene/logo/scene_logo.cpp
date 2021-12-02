#include "c_pch.h"
#include "scene_logo.h"

#include <codecvt>
#include <iostream>
#include <locale>

#include "src/game_object/back_ground/back_ground.h"
#include "src/game_object/ui/login/text_box_ui.h"
#include "src/network/server_packet_handler.h"
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

	auto a = InputDevice::GetInstance().GetDirectKeyState(DIK_RETURN);
	if (a & 0x80)
	{
		/*	const auto scene = SceneLoading::Create(_graphic_device, kScene::kSceneGamePlay0);
			if (SUCCEEDED(SceneManager::GetInstance().SetUpScene(scene)))
			{
				GameLogicManager::Clear(static_cast<uint32_t>(kScene::kSceneLogo));
			}*/
		auto& objectManager = ObjectManager::GetInstance();
		Protocol::ClientLogin pkt;
		auto id = std::static_pointer_cast<TextBoxUi>(objectManager.GetGameObjectPtr(static_cast<int32_t>(kScene::kSceneLogo),
			TEXT("Layer_LoginBox"),
			TEXT("Prototype_BackGround"),
			0));
		auto pw = std::static_pointer_cast<TextBoxUi>(objectManager.GetGameObjectPtr(static_cast<int32_t>(kScene::kSceneLogo),
			TEXT("Layer_LoginBox"),
			TEXT("Prototype_BackGround"),
			1));
		if (id && pw)
		{
			pkt.set_id(FileUtils::ConvertWtoC(id->GetText().c_str()).c_str());
			pkt.set_pw(FileUtils::ConvertWtoC(pw->GetText().c_str()).c_str());
		}
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
	if (FAILED(ObjectManager::GetInstance().AddGameObject(static_cast<int32_t>(kScene::kSceneLogo), TEXT("Prototype_BackGround"), layerTag)))
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
