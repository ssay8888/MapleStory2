#pragma once
class GameObject;
class Component;
class Layer;

class ObjectManager
{
private:
	ObjectManager();
	~ObjectManager();

public:
	static auto GetInstance()->ObjectManager&
	{
		static ObjectManager instance;
		return instance;
	}

public:
	auto GetComponentPtr(uint32_t levelIndex, const std::wstring& layerTag, const std::wstring& componentTag, uint32_t index = 0) const ->std::shared_ptr<Component>;
	auto GetGameObjectPtr(uint32_t levelIndex, const std::wstring& layerTag, uint32_t iIndex)->std::shared_ptr<GameObject>;

public:
	auto ReserveContainer(uint32_t numLevel)->HRESULT;
	auto AddPrototype(const std::wstring& prototypeTag, const std::shared_ptr<GameObject>& prototype)->HRESULT;
	auto AddGameObject(uint32_t levelIndex, const std::wstring& prototypeTag, const std::wstring& layerTag, void* arg = nullptr)->HRESULT;
	auto AddGameObject(uint32_t levelIndex, const std::wstring& layerTag, std::shared_ptr<GameObject> gameObject)->HRESULT;

	auto Tick(double timeDelta) const ->int32_t;
	auto LateTick(double timeDelta) const ->int32_t;
	auto LayerClear(uint32_t levelIndex, const std::wstring& layerTag)->void;
	auto Clear(uint32_t levelIndex) const ->void;


private:
	auto FindPrototype(const std::wstring& prototypeTag)->std::shared_ptr<GameObject>;
	auto FindLayer(uint32_t levelIndex, const std::wstring& layerTag) const ->std::shared_ptr<Layer>;

private:
	std::map<std::wstring, std::shared_ptr<GameObject>>			_prototypes;
	typedef std::map<std::wstring, std::shared_ptr<GameObject>> Prototypes;

	std::map<std::wstring, std::shared_ptr<Layer>>* _layers = nullptr;
	typedef std::map<std::wstring, std::shared_ptr<Layer>> Layers;

private:
	uint32_t _num_level = 0;

};

