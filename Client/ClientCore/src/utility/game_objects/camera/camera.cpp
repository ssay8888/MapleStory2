#include "pch.h"
#include "camera.h"

#include "src/utility/components/transform/transform.h"
#include "src/utility/pipe_line/pipe_line.h"

Camera::Camera(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT Camera::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Camera::NativeConstruct(void* arg)
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

	_transform->SetState(Transform::kState::kStateRight, vRight);
	_transform->SetState(Transform::kState::kStateUp, vUp);
	_transform->SetState(Transform::kState::kStateLook, vLook);
	_transform->SetState(Transform::kState::kStatePosition, _camera_desc.eye);

	SetTransform();
	return S_OK;
}

int32_t Camera::Tick(const double timeDelta)
{
	GameObject::Tick(timeDelta);

	SetTransform();

	return int32_t();
}

int32_t Camera::LateTick(const double timeDelta)
{
	GameObject::LateTick(timeDelta);

	return int32_t();
}

HRESULT Camera::Render()
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