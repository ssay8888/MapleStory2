#pragma once
#include "src/utility/game_objects/game_object.h"

class ViBufferRect;
class Transform;
class Texture;

class Effect final : public GameObject
{
public:
	explicit Effect(const ComPtr<IDirect3DDevice9>& device)
		: GameObject(device)
	{
	}
	virtual ~Effect() = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) ->HRESULT override;
	auto LateTick(double timeDelta) ->HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Effect>;

private:

	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Texture>		_texture_com = nullptr;
	std::shared_ptr<Transform>		_transform_com = nullptr;
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;

private:
	float					m_fFrame = 0.f;

};

