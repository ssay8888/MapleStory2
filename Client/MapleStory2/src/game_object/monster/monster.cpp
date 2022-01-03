#include "c_pch.h"
#include "monster.h"

#include "data_reader/data_reader_manager.h"
#include "stats/monster_stats.h"

Monster::Monster(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT Monster::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Monster::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);
	if (arg == nullptr)
	{
		return E_FAIL;
	}

	_monster_info = *static_cast<std::shared_ptr<DataReaderManager::MonsterInfo>*>(arg);

	if (_monster_info == nullptr)
	{
		return E_FAIL;
	}
	_monster_stat = MakeShared<MonsterStats>(_monster_info);
	return S_OK;
}

int32_t Monster::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t Monster::LateTick(const double timeDelta)
{
	return GameObject::LateTick(timeDelta);
}

HRESULT Monster::Render()
{
	return GameObject::Render();
}

auto Monster::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = MakeShared<Monster>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto Monster::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Monster>
{
	auto instance = MakeShared<Monster>(device);
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}
