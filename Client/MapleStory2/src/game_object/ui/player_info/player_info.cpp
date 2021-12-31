#include "c_pch.h"
#include "player_info.h"

PlayerInfo::PlayerInfo(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT PlayerInfo::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT PlayerInfo::NativeConstruct(void* arg)
{
	return GameObject::NativeConstruct(arg);
}

int32_t PlayerInfo::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t PlayerInfo::LateTick(const double timeDelta)
{
	return GameObject::LateTick(timeDelta);
}

HRESULT PlayerInfo::Render()
{
	return GameObject::Render();
}

auto PlayerInfo::Clone(void* arg) -> std::shared_ptr<GameObject>
{
}

auto PlayerInfo::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<PlayerInfo>
{
}

auto PlayerInfo::AddComponents() -> HRESULT
{
}
