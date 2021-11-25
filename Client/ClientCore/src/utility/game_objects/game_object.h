#pragma once
class Component;

class GameObject : public std::enable_shared_from_this<GameObject>
{
protected:
	explicit GameObject(const ComPtr<IDirect3DDevice9>& device);
	virtual ~GameObject() = default;

public:
	auto AddComponent(int32_t levelIndex, const std::wstring& prototypeTag, const std::wstring& componentTag, std::shared_ptr<Component>* ppOut, void* arg = nullptr)->HRESULT;
	auto GetComponentPtr(const std::wstring& componentTag)->std::shared_ptr<Component>;
 
public:
	virtual auto NativeConstructPrototype()->HRESULT;
	virtual auto NativeConstruct(void* arg)->HRESULT;
	virtual auto Tick(double timeDelta)->int32_t;
	virtual auto LateTick(double timeDelta)->int32_t;
	virtual auto Render()->HRESULT;

public:
	virtual auto Clone(void* arg = nullptr)->std::shared_ptr<GameObject> = 0;

private:
	auto FindComponent(const std::wstring& componentTag)->std::shared_ptr<Component>; 

protected:
	ComPtr<IDirect3DDevice9> _graphic_device = nullptr;

protected:
	std::map<std::wstring, std::shared_ptr<Component>> _components;
	typedef std::map<std::wstring, std::shared_ptr<Component>> Components;
;


};

