#pragma once
#include "src/utility/game_objects/camera/camera.h"

class IngameCamera final : public Camera
{
public:
	explicit IngameCamera(const ComPtr<IDirect3DDevice9>& device);
	virtual ~IngameCamera() = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;

private:
	HRESULT AddComponents();

private:
	std::shared_ptr<Transform> _target_transform_com = nullptr;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<IngameCamera>;
	auto Clone(void* arg = nullptr)->std::shared_ptr<GameObject> override;

private:
	bool _free_camera = false;
};

