#pragma once
#include "src/utility/game_objects/game_object.h"

class ViBufferRect;
class Texture;
class Shader;
class Transform;

class BackGround final : public GameObject
{
public:
	struct BackGroundInfo
	{
		std::wstring prototype_texture_name;
		int32_t pos_x{};
		int32_t pos_y{};
		int32_t image_size_x{};
		int32_t image_size_y{};
	};
	explicit BackGround(const ComPtr<IDirect3DDevice9>& device);
	virtual ~BackGround() = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT  override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT  override;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<BackGround>;
	auto Clone(void* arg = nullptr)->std::shared_ptr<GameObject> override;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Shader>			_shader_com = nullptr;
	std::shared_ptr<Texture>		_texture_com = nullptr;
	std::shared_ptr<Transform>		_transform_com = nullptr;
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;
	BackGroundInfo					_info;
};
