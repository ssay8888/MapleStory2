#pragma once
class Transform;
class Weapon;

class WeaponManager
{
private:
	WeaponManager();
public:
	static auto GetInstance()->WeaponManager&
	{
		static WeaponManager instance;
		return instance;
	}

public:
	auto AddWeapon(int64_t playerId, int32_t itemId, std::shared_ptr<Transform> transform)->HRESULT;
	auto RemoveWeapon(int64_t playerId)->HRESULT;
	auto LateTick()->HRESULT;
	auto Render()->HRESULT;

private:
	auto NativeConstruct()->void;

private:
	std::map<int64_t, std::shared_ptr<Weapon>> _weapons;
};

