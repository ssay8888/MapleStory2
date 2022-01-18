#include "pch.h"
#include "camera.h"

#include "src/utility/components/transform/transform.h"
#include "src/utility/pipe_line/pipe_line.h"

Camera::Camera(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

auto Camera::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto Camera::NativeConstruct(void* arg) -> HRESULT
{
	GameObject::NativeConstruct(arg);


	if (nullptr != arg)
	{
		memcpy(&_camera_desc, arg, sizeof(Camera::CameraDesc));
	}

	_float3		vLook = (_camera_desc.at - _camera_desc.eye);
	D3DXVec3Normalize(&vLook, &vLook);

	_float3		vRight;
	D3DXVec3Cross(&vRight, &_camera_desc.axis_y, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	_float3		vUp;
	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	_matrix matrix;
	D3DXMatrixIdentity(&matrix);
	_transform->SetState(Transform::kState::kStateRight, vRight);
	_transform->SetState(Transform::kState::kStateUp, vUp);
	_transform->SetState(Transform::kState::kStateLook, vLook);
	_transform->SetState(Transform::kState::kStatePosition, _camera_desc.eye);


	//_transform->SetState(Transform::kState::kStateRight, *((_float3*)&matrix.m[0][0]));
	//_transform->SetState(Transform::kState::kStateUp, *((_float3*)&matrix.m[1][0]));
	//_transform->SetState(Transform::kState::kStateLook, *((_float3*)&matrix.m[2][0]));


	/*

	if (0 != mouseMove)
		_transform->RotationAxis(_float3(0.f, 1.f, 0.f), timeDelta * mouseMove * 0.02f);

	mouseMove = InputDevice::GetInstance().GetDirectMouseMoveState(InputDevice::kDirectInMouseState::kY);

	if (0 != mouseMove)
		_transform->RotationAxis(_transform->GetState(Transform::kState::kStateRight), timeDelta * mouseMove * 0.02f);
	*/


	_float3 axis{ 0, 1,  0 };
	//_transform->RotationAxis(axis, 1, D3DXToRadian(90.f));

	axis = { 1, 0, 0 };
	//_transform->RotationAxis(axis, 1, D3DXToRadian(20.f));

	axis = { 0, 0, 1 };
	//_transform->RotationAxis(axis, 1, D3DXToRadian(360 - 3.21383667f));

	SetTransform();
	return S_OK;
}

auto Camera::Tick(const double timeDelta) -> HRESULT
{
	GameObject::Tick(timeDelta);

	SetTransform();

	return int32_t();
}

auto Camera::LateTick(const double timeDelta) -> HRESULT
{
	GameObject::LateTick(timeDelta);

	return int32_t();
}

auto Camera::Render() -> HRESULT
{
	return S_OK;
}

auto Camera::SetTransform() -> HRESULT
{
	_matrix		projMatrix;

	auto& pipeInstance = PipeLine::GetInstance();

	pipeInstance.SetTransform(D3DTS_VIEW, _transform->GetWorldMatrixInverse());

	D3DXMatrixPerspectiveFovLH(&projMatrix, _camera_desc.fovy, _camera_desc.aspect, _camera_desc.near_, _camera_desc.far_);
	pipeInstance.SetTransform(D3DTS_PROJECTION, projMatrix);

	return S_OK;
}