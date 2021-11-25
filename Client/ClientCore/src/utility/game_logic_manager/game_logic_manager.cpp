#include "pch.h"
#include "game_logic_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/scene_utility/scene_manager.h"

HRESULT GameLogicManager::InitDevice(const HINSTANCE hInst, const HWND hWnd, const int32_t numLevel)
{

	if (FAILED(InputDevice::GetInstance().InitDevice(hInst, hWnd)))
	{
		return E_FAIL;
	}

	if (FAILED(ObjectManager::GetInstance().ReserveContainer(numLevel)))
	{
		return E_FAIL;
	}

	if (FAILED(ComponentManager::GetInstance().ReserveContainer(numLevel)))
	{
		return E_FAIL;
	}

	return S_OK;
}

auto GameLogicManager::Clear(const uint32_t levelIndex) -> void
{
	ObjectManager::GetInstance().Clear(levelIndex);
	ComponentManager::GetInstance().Clear(levelIndex);
}

auto GameLogicManager::Tick(const double timeDelta) -> int32_t
{
	InputDevice::GetInstance().InvalidateInputDevice();

	int32_t progress = SceneManager::GetInstance().Tick(timeDelta);

	progress = ObjectManager::GetInstance().Tick(timeDelta);

	return int32_t();
}

auto GameLogicManager::LateTick(const double timeDelta) -> int32_t
{
	const int32_t progress = ObjectManager::GetInstance().LateTick(timeDelta);
	return progress;
}
