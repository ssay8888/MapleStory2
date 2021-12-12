#include "pch.h"
#include "object_manager.h"

#include "layer/layer.h"
#include "src/utility/game_objects/game_object.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
	delete[] _layers;
}

auto ObjectManager::GetComponentPtr(const uint32_t levelIndex, const std::wstring& layerTag,
                                    const std::wstring& componentTag, uint32_t index) const -> std::shared_ptr<Component>
{
	const std::shared_ptr<Layer> layer = FindLayer(levelIndex, layerTag);
	if (layer == nullptr)
	{
		return nullptr;
	}

	return layer->GetComponentPtr(componentTag, index);
}

auto ObjectManager::GetGameObjectPtr(uint32_t levelIndex, const std::wstring& layerTag, uint32_t iIndex) -> std::shared_ptr<GameObject>
{
		const std::shared_ptr<Layer> layer = FindLayer(levelIndex, layerTag);
		if (nullptr == layer)
			return nullptr;


		return layer->GetGameObject(iIndex);
}

auto ObjectManager::ReserveContainer(const uint32_t numLevel) -> HRESULT
{
	if (_layers)
	{
		return E_FAIL;
	}

	_layers = new Layers[numLevel];
	_num_level = numLevel;
	return S_OK;
}

auto ObjectManager::AddPrototype(const std::wstring& prototypeTag, const std::shared_ptr<GameObject>& prototype) -> HRESULT
{
	if (prototype == nullptr || nullptr != FindPrototype(prototypeTag))
	{
		return E_FAIL;
	}

	_prototypes.emplace(prototypeTag, prototype);
	return S_OK;
}

auto ObjectManager::AddGameObject(const uint32_t levelIndex, const std::wstring& prototypeTag, const std::wstring& layerTag,
	void* arg) -> HRESULT
{
	if (_num_level <= levelIndex)
	{
		return E_FAIL;
	}

	const std::shared_ptr<GameObject> prototype = FindPrototype(prototypeTag);
	if (nullptr == prototype)
	{
		return E_FAIL;
	}

	const std::shared_ptr<GameObject> gameObject = prototype->Clone(arg);
	if (gameObject == nullptr)
	{
		return E_FAIL;
	}

	std::shared_ptr<Layer> layer = FindLayer(levelIndex, layerTag);

	if (layer == nullptr)
	{
		layer = std::make_shared<Layer>();

		layer->AddGameObject(gameObject);

		_layers[levelIndex].emplace(layerTag, layer);
	}
	else
	{
		layer->AddGameObject(gameObject);
	}
	return S_OK;
}

auto ObjectManager::AddGameObject(uint32_t levelIndex, const std::wstring& layerTag, std::shared_ptr<GameObject> gameObject) -> HRESULT
{
	if (_num_level <= levelIndex || gameObject == nullptr)
	{
		return E_FAIL;
	}

	std::shared_ptr<Layer> layer = FindLayer(levelIndex, layerTag);

	if (layer == nullptr)
	{
		layer = std::make_shared<Layer>();

		layer->AddGameObject(gameObject);

		_layers[levelIndex].emplace(layerTag, layer);
	}
	else
	{
		layer->AddGameObject(gameObject);
	}
	return S_OK;
}

auto ObjectManager::Tick(const double timeDelta) const -> int32_t
{
	for (uint32_t i = 0; i < _num_level; ++i)
	{
		for (const auto& [layerTag, objectPtr] : _layers[i])
		{
			objectPtr->Tick(timeDelta);
		}
	}
	return S_OK;
}

auto ObjectManager::LateTick(const double timeDelta) const -> int32_t
{
	for (uint32_t i = 0; i < _num_level; ++i)
	{
		for (const auto& [layerTag, objectPtr] : _layers[i])
		{
			objectPtr->LateTick(timeDelta);
		}
	}
	return S_OK;
}

auto ObjectManager::LayerClear(uint32_t levelIndex, const std::wstring& layerTag)->void
{
	auto iter = _layers[levelIndex].find(layerTag);
	if (iter != _layers[levelIndex].end())
	{
		iter->second->Clear();
	}
}

auto ObjectManager::Clear(const uint32_t levelIndex) const -> void
{
	_layers[levelIndex].clear();
}

auto ObjectManager::FindPrototype(const std::wstring& prototypeTag) -> std::shared_ptr<GameObject>
{
	const auto iterator = std::find_if(_prototypes.begin(), _prototypes.end(), TagFinder(prototypeTag));

	if (iterator == _prototypes.end())
	{
		return nullptr;
	}

	return iterator->second;
}

auto ObjectManager::FindLayer(const uint32_t levelIndex, const std::wstring& layerTag) const -> std::shared_ptr<Layer>
{
	if (_num_level <= levelIndex)
	{
		return nullptr;
	}

	const auto iterator = std::find_if(_layers[levelIndex].begin(), _layers[levelIndex].end(), TagFinder(layerTag));
	if (iterator == _layers[levelIndex].end())
	{
		return nullptr;
	}

	return iterator->second;
}
