#pragma once
#include "src/utility/game_objects/game_object.h"

class ViBufferCube;
class Transform;
class Texture;

class Sky final : public GameObject
{
public:
	explicit Sky(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Sky() = default;
public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;

private:
	std::shared_ptr<Texture> _texture_com = nullptr;
	std::shared_ptr<Transform> _transform_com = nullptr;
	std::shared_ptr<ViBufferCube> _vi_buffer_com = nullptr;

private:
	auto AddComponents()->HRESULT;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Sky>;
	virtual auto Clone(void* arg)->std::shared_ptr<GameObject> override;
};

