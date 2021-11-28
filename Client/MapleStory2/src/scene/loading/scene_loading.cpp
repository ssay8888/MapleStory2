#include "pch.h"
#include "scene_loading.h"

#include "loader/loading.h"
#include "src/scene/game_play/game_play.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/scene_utility/scene_manager.h"

SceneLoading::SceneLoading(const ComPtr<IDirect3DDevice9>& device):
	Scene(device)
{
}

auto SceneLoading::NativeConstruct(const kScene nextScene) -> HRESULT
{
	_next_scene = nextScene;

	/* 로딩씬이 생성될때 로더를 하나 생성하낟. */
	/* 다음넘어갈씬에 필요한 객체원형, 자원원형 생성. */
	_loader = Loading::Create(_graphic_device, nextScene);

	if (nullptr == _loader)
		return E_FAIL;

	/* 실제 게임내에 출현할 객체의 복사본을 새엇ㅇ한다. */
	if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(ReadyLayerUserInterface(TEXT("Layer_UserInterface"))))
		return E_FAIL;

	return S_OK;
}

int32_t SceneLoading::Tick(const double timeDelta)
{
	if (_loader->IsFinished())
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x0001)
		{
			std::shared_ptr<Scene> scene = nullptr;

			switch (_next_scene)
			{
			case kScene::kSceneGamePlay0:
				scene = GamePlay::Create(_graphic_device);
				break;
			case kScene::kSceneGamePlay1:
				break;
			default: 
				break;
			}

			//pGameInstance->Clear(LEVEL_LOADING);
			GameLogicManager::Clear(static_cast<uint32_t>(kScene::kSceneLogo));
			SceneManager::GetInstance().SetUpScene(scene);

		}
	}
	return Scene::Tick(timeDelta);
}

HRESULT SceneLoading::Render()
{
	SetWindowText(g_Wnd, _loader->GetSystemMessage().c_str());
	return Scene::Render();
}

std::shared_ptr<SceneLoading> SceneLoading::Create(const ComPtr<IDirect3DDevice9>& device, kScene nextLevel)
{
	auto instance = std::make_shared<SceneLoading>(device);

	if (FAILED(instance->NativeConstruct(nextLevel)))
	{
		MSGBOX("Failed to Creating LevelLoading");
		return nullptr;
	}
	return instance;
}

auto SceneLoading::ReadyLayerBackGround(const std::wstring& layerTag) -> HRESULT
{
	return S_OK;
}

auto SceneLoading::ReadyLayerUserInterface(const std::wstring& layerTag) -> HRESULT
{
	return S_OK;
}
