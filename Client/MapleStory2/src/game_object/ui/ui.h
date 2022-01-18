#pragma once
#include "src/utility/game_objects/game_object.h"

class ViBufferRect;
class Texture;

class Ui : public GameObject
{
public:
	explicit Ui(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Ui() = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Ui>;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Texture>		_texture_com = nullptr;
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;

private:
	_matrix				_proj_matrix;
	float				_x, _y, _size_x, _size_y;


	
};

