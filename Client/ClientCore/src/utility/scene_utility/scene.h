#pragma once
class Scene : public std::enable_shared_from_this<Scene>
{
protected:
	explicit Scene(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Scene() = default;

public:
	virtual auto NativeConstruct()->HRESULT;
	virtual auto Tick(double timeDelta)->int32_t;
	virtual auto Render()->HRESULT;

protected:
	ComPtr<IDirect3DDevice9>		_graphic_device = nullptr;
};

