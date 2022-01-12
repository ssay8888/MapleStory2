#include "c_pch.h"
#include "weapon_manager.h"

#include "src/utility/game_objects/manager/object_manager.h"
#include "src/game_object/item/Weapon.h"

WeaponManager::WeaponManager()
{
	NativeConstruct();
}

auto WeaponManager::NativeConstruct() -> void
{
}

auto WeaponManager::AddWeapon(int64_t playerId, int32_t itemId, std::shared_ptr<Transform> transform) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	std::wstring prototypeName(L"Prototype_ItemModel_");
	prototypeName.append(std::to_wstring(itemId)).append(L"_0");
	Weapon::WeaponInfo info;
	info.item_id = itemId;
	info.player_id = playerId;
	info.transform = transform;
	auto item = std::static_pointer_cast<Weapon>(objectManager.CreateGameObject(kSceneStatic, TEXT("Prototype_Mesh_Weapon"), &info));

	if (item)
	{
		return _weapons.emplace(playerId, item).second ? S_OK : E_FAIL;
	}
	return E_FAIL;
}

auto WeaponManager::RemoveWeapon(const int64_t playerId) -> HRESULT
{
	auto iterator = _weapons.find(playerId);
	if (iterator != _weapons.end())
	{
		_weapons.erase(iterator);
		return S_OK;
	}
	return E_FAIL;
}

auto WeaponManager::LateTick() -> HRESULT
{
	for (const auto& weapon : _weapons)
	{
		weapon.second->LateTick(0.f);
	}
	return S_OK;
}

auto WeaponManager::Render() -> HRESULT
{
	for (const auto& weapon : _weapons)
	{
		weapon.second->Render();
	}
	return S_OK;
}
