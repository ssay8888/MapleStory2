#include "c_pch.h"
#include "equipped.h"

#include "src/system/graphic/graphic_device.h"

Equipped::Equipped():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

HRESULT Equipped::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Equipped::NativeConstruct(void* arg)
{
	return GameObject::NativeConstruct(arg);
}

int32_t Equipped::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t Equipped::LateTick(const double timeDelta)
{
	return GameObject::LateTick(timeDelta);
}

HRESULT Equipped::Render()
{
	return GameObject::Render();
}

auto Equipped::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<Equipped>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed To Clone Equipped");
		return nullptr;
	}
	return instance;
}

auto Equipped::Create() -> std::shared_ptr<Equipped>
{
	auto instance = std::make_shared<Equipped>();
	if (FAILED(instance->NativeConstructPrototype()))
	{
		MSGBOX("Failed To Create Equipped");
		return nullptr;
	}
	return instance;
}

auto Equipped::FindItem(const GameContents::kEquipeType type) -> int32_t
{
	auto item = _eqp_item.find(type);
	if (item != _eqp_item.end())
	{
		return item->second;
	}
	return 0;
}

auto Equipped::AddItem(GameContents::kEquipeType type, int32_t itemId) -> void
{
	_eqp_item.emplace(type, itemId);
}

auto Equipped::GetAllItem() -> std::vector<int32_t>
{
	std::vector<int32_t> items;
	for (auto& item : _eqp_item)
	{
		items.push_back(item.second);
	}
	return items;
}
