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
	HRESULT AddGameObject(const std::shared_ptr<GameObject>& gameObject);
	auto GetGameObject(uint32_t index)->std::shared_ptr<GameObject>;
	int32_t Tick(double timeDelta);
	int32_t LateTick(double timeDelta);

private:
	std::vector<std::shared_ptr<GameObject>>	_objects;


};

