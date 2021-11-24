#include "pch.h"
#include "scene_manager.h"

#include "scene.h"

auto SceneManager::SetUpScene(const std::shared_ptr<Scene>& level) -> HRESULT
{
	_current_scene = level;
	return S_OK;
}

auto SceneManager::Tick(const double timeDelta) const -> int32_t
{
	if (_current_scene == nullptr)
		return -1;

	return _current_scene->Tick(timeDelta);
}

auto SceneManager::Render() const -> HRESULT
{
	if (_current_scene == nullptr)
		return -1;

	return _current_scene->Render();
}
