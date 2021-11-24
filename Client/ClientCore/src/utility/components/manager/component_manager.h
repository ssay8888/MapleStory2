#pragma once
class Component;

class ComponentManager
{
private:
	ComponentManager();
	~ComponentManager();

public:
	static auto GetInstance() -> ComponentManager&
	{
		static ComponentManager instance;
		return instance;
	}

public:
	auto ReserveContainer(uint32_t numLevels)->HRESULT;
	auto AddPrototype(uint32_t levelIndex, const std::wstring& prototypeTag, const std::shared_ptr<Component>& prototype) const->HRESULT;
	auto CloneComponent(uint32_t levelIndex, const std::wstring& prototypeTag, void* arg = nullptr) const->std::shared_ptr<Component>;
	auto Clear(uint32_t levelIndex) const ->void;

private:
	std::map<const std::wstring, std::shared_ptr<Component>>*			_prototypes = nullptr;
	typedef std::map<const std::wstring, std::shared_ptr<Component>>	Prototypes;

private:
	uint32_t		_num_levels = 0;

private:
	auto FindComponent(uint32_t levelIndex, const std::wstring& prototypeTag) const->std::shared_ptr<Component>;
};

