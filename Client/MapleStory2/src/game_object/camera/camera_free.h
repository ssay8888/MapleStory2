#pragma once
#include "src/utility/game_objects/camera/camera.h"

class CameraFree final : public Camera
{
public:
	explicit CameraFree(const ComPtr<IDirect3DDevice9>& device);
	virtual ~CameraFree() = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;

private:
	HRESULT AddComponents();

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<CameraFree>;
	auto Clone(void* arg = nullptr)->std::shared_ptr<GameObject> override;
};

