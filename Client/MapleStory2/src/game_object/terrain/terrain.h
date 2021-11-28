#pragma once
#include "src/utility/game_objects/game_object.h"

class Shader;
class Texture;
class Transform;
class ViBufferTerrain;

class Terrain final : public GameObject
{
public:
	Terrain(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Terrain() = default;
public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> int32_t override;
	auto LateTick(double timeDelta) -> int32_t override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Terrain>;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Shader> _shader_com					= nullptr;
	std::shared_ptr<Texture> _filter_com				= nullptr;
	std::shared_ptr<Texture> _texture_com				= nullptr;
	std::shared_ptr<Transform> _transform_com			= nullptr;
	std::shared_ptr<ViBufferTerrain> _vi_buffer_com		= nullptr;
};

