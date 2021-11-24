#include "pch.h"
#include "game_object.h"

#include "src/utility/components/manager/component_manager.h"

GameObject::GameObject(const ComPtr<IDirect3DDevice9>& device)
{
}

GameObject::GameObject(const GameObject& rhs)
{
}

auto GameObject::AddComponent(int32_t levelIndex, const std::wstring prototypeTag, const std::wstring componentTag,
	std::shared_ptr<Component>* ppOut, void* arg) -> HRESULT
{
	if (nullptr != FindComponent(componentTag))
		return E_FAIL;

	auto component = ComponentManager::GetInstance().CloneComponent(levelIndex, prototypeTag, arg);

	_components.emplace(componentTag, component);

	*ppOut = component;

	return S_OK;
}

auto GameObject::GetComponentPtr(const std::wstring& componentTag) -> std::shared_ptr<Component>
{
	return FindComponent(componentTag);
}

auto GameObject::NativeConstructPrototype() -> HRESULT
{
	return S_OK;
}

auto GameObject::NativeConstruct(void* arg) -> HRESULT
{
	return S_OK;
}

auto GameObject::Tick(double timeDelta) -> int32_t
{
	return int32_t();
}

auto GameObject::LateTick(double timeDelta) -> int32_t
{
	return int32_t();
}

auto GameObject::Render() -> HRESULT
{
	return S_OK;
}

auto GameObject::FindComponent(const std::wstring& componentTag) -> std::shared_ptr<Component>
{
	const auto	iter = std::find_if(_components.begin(), _components.end(), TagFinder(componentTag.c_str()));

	if (iter == _components.end())
		return nullptr;

	return iter->second;
}
