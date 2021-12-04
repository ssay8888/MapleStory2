#pragma once
#include "src/utility/game_objects/game_object.h"

class Transform;

class Camera : public GameObject
{
public:
	using CameraDescription = struct CameraDesc
	{
		_float3		eye; /*ī�Ӹ޶� ��ġ. */
		_float3		at; /* ī�޶� ���� ��. */
		_float3		axis_y = _float3(0.f, 1.f, 0.f);

		float		fovy{};
		float		aspect{};
		float		near_{};
		float		far_{};
	};

public:
	explicit Camera(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Camera() = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;

private:
	auto SetTransform()->HRESULT;

public:
	virtual auto Clone(void* arg = nullptr)->std::shared_ptr<GameObject> = 0;

protected:
	CameraDescription					_camera_desc;
	std::shared_ptr<Transform>			_transform = nullptr;

};

