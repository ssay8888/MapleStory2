#pragma once
class GameObject;
class Component;

class Layer
{
public:
	Layer() = default;
	~Layer() = default;

public:
	auto GetComponentPtr(const std::wstring& componentTag, const uint32_t index)->std::shared_ptr<Component>;

public:
	auto AddGameObject(const std::shared_ptr<GameObject>& gameObject) -> HRESULT;
	auto GetGameObject(uint32_t index)->std::shared_ptr<GameObject>;
	auto Tick(double timeDelta) -> int32_t;
	auto LateTick(double timeDelta) -> int32_t;
	auto RemoveAllDeadObject()->void;
	auto Clear()->void;

private:
	std::vector<std::shared_ptr<GameObject>>	_objects;


};

