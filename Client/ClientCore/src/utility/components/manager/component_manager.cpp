#include "pch.h"
#include "component_manager.h"

#include "src/utility/components/component.h"


ComponentManager::ComponentManager()
{
}

ComponentManager::~ComponentManager()
{
	for (uint32_t i = 0; i < _num_levels; ++i)
	{
		_prototypes[i].clear();
	}
	delete[] _prototypes;
}

auto ComponentManager::ReserveContainer(uint32_t numLevels) -> HRESULT
{
	if (nullptr != _prototypes)
		return E_FAIL;

	_prototypes = new Prototypes[numLevels];

	_num_levels = numLevels;

	return S_OK;
}

auto ComponentManager::AddPrototype(const uint32_t levelIndex, const std::wstring& prototypeTag,
                                    const std::shared_ptr<Component>& prototype) const -> HRESULT
{
	if (nullptr == prototype ||
		_num_levels <= levelIndex ||
		nullptr != FindComponent(levelIndex, prototypeTag))
		return E_FAIL;

	_prototypes[levelIndex].emplace(prototypeTag, prototype);
	return S_OK;  
}

auto ComponentManager::CloneComponent(const uint32_t levelIndex, const std::wstring& prototypeTag,
                                      void* arg) const -> std::shared_ptr<Component>
{
	if (_num_levels <= levelIndex)
		return nullptr;

	const std::shared_ptr<Component> prototype = FindComponent(levelIndex, prototypeTag);

	if (nullptr == prototype)
		return nullptr;

	auto pComponent = prototype->Clone(arg);
	return pComponent;

}

auto ComponentManager::Clear(uint32_t levelIndex) const -> void
{
	_prototypes[levelIndex].clear();
}

std::shared_ptr<Component> ComponentManager::FindComponent(const uint32_t levelIndex, const std::wstring& prototypeTag) const
{
	const auto	iter = std::find_if(_prototypes[levelIndex].begin(), _prototypes[levelIndex].end(), TagFinder(prototypeTag.c_str()));

	if (iter == _prototypes[levelIndex].end())
		return nullptr;

	return iter->second;
}
