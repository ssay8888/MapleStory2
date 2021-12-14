#include "pch.h"
#include "game_object.h"

#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/pipe_line/pipe_line.h"

GameObject::GameObject(const ComPtr<IDirect3DDevice9>& device):
	_graphic_device(device)
{
}

auto GameObject::AddComponent(const int32_t levelIndex, const std::wstring& prototypeTag, const std::wstring& componentTag,
                              std::shared_ptr<Component>* ppOut, void* arg) -> HRESULT
{
	if (nullptr != FindComponent(componentTag))
	{
		return E_FAIL;
	}

	auto component = ComponentManager::GetInstance().CloneComponent(levelIndex, prototypeTag, arg);
	if (component == nullptr)
	{
		return E_FAIL;
	}
	_components.emplace(componentTag, component);

	*ppOut = component;

	return S_OK;
}

auto GameObject::GetComponentPtr(const std::wstring& componentTag) -> std::shared_ptr<Component>
{
	return FindComponent(componentTag);
}

auto GameObject::CloneComponent(int32_t levelIndex, const std::wstring& prototypeTag, const std::wstring& componentTag,
	void* arg) -> std::shared_ptr<Component>
{
	auto component = ComponentManager::GetInstance().CloneComponent(levelIndex, prototypeTag, arg);
	return component;
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

void GameObject::ComputeCameraDistance(const std::shared_ptr<Transform>& transform)
{
	const _float3	worldPosition = transform->GetState(Transform::kState::kStatePosition);

	const _matrix	viewMatrix = PipeLine::GetInstance().GetTransform(D3DTS_VIEW);

	_matrix	camWorldMatrix = *D3DXMatrixInverse(&camWorldMatrix, nullptr, &viewMatrix);

	const _float3	camPosition = *reinterpret_cast<_float3*>(&camWorldMatrix.m[3][0]);

	const auto len = worldPosition - camPosition;
	_cam_distance = D3DXVec3Length(&len);
}

void GameObject::ComputeViewZ(const std::shared_ptr<Transform>& transform)
{
	_float3	vWorldPosition = transform->GetState(Transform::kState::kStatePosition);

	const _matrix	ViewMatrix = PipeLine::GetInstance().GetTransform(D3DTS_VIEW);

	D3DXVec3TransformCoord(&vWorldPosition, &vWorldPosition, &ViewMatrix);

	_cam_distance = vWorldPosition.z;
}

auto GameObject::FindComponent(const std::wstring& componentTag) -> std::shared_ptr<Component>
{
	const auto	iter = std::find_if(_components.begin(), _components.end(), TagFinder(componentTag.c_str()));

	if (iter == _components.end())
		return nullptr;

	return iter->second;
}
