#include "c_pch.h"
#include "equipped.h"

#include "src/system/graphic/graphic_device.h"

Equipped::Equipped():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

auto Equipped::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto Equipped::NativeConstruct(void* arg) -> HRESULT
{
	return GameObject::NativeConstruct(arg);
}

auto Equipped::Tick(const double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto Equipped::LateTick(const double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto Equipped::Render() -> HRESULT
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

auto Equipped::RemoveItem(GameContents::kEquipeType type) -> void
{
	_eqp_item.erase(type);
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
