#include "pch.h"
#include "layer.h"

#include "src/utility/game_objects/game_object.h"

auto Layer::GetComponentPtr(const std::wstring& componentTag, const uint32_t index) -> std::shared_ptr<Component>
{
	if (index >= _objects.size())
	{
		return nullptr;
	}

	auto iterator = _objects.begin();

	for (uint32_t i = 0; i < index; ++i)
		++iterator;

	return (*iterator)->GetComponentPtr(componentTag);
}

HRESULT Layer::AddGameObject(const std::shared_ptr<GameObject>& gameObject)
{
	if (gameObject == nullptr)
	{
		return E_FAIL;
	}

	_objects.push_back(gameObject);
	return S_OK;
}

auto Layer::GetGameObject(const uint32_t index) -> std::shared_ptr<GameObject>
{
	if (index >= _objects.size())
		return nullptr;

	auto iter = _objects.begin();

	iter += index;

	return (*iter);
}

int32_t Layer::Tick(const double timeDelta)
{
	if (_objects.empty())
	{
		return S_OK;
	}
	for (const auto& gameObject : _objects)
	{
		if (gameObject)
		{
			gameObject->Tick(timeDelta);
		}
	}
	return S_OK;
}

int32_t Layer::LateTick(const double timeDelta)
{
	if (_objects.empty())
	{
		return S_OK;
	}
	for (const auto& gameObject : _objects)
	{
		if (gameObject)
		{
			gameObject->LateTick(timeDelta);
		}
	}
	return S_OK;
}

auto Layer::RemoveAllDeadObject() -> void
{
	for (auto iterator = _objects.begin(); iterator != _objects.end();)
	{
		if ((*iterator)->IsDead())
		{
			iterator = _objects.erase(iterator);
			continue;
		}
		++iterator;
	}
}

auto Layer::Clear() -> void
{
	_objects.clear();
}
