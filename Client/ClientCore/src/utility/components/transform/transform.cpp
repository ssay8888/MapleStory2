#include "pch.h"
#include "transform.h"

#include "src/utility/components/vi_buffer/vi_buffer_terrain/vi_buffer_terrain.h"

Transform::Transform(const ComPtr<IDirect3DDevice9>& graphicDevice):
	Component(graphicDevice),
	_transform_state()
{
	NativeConstructPrototype();
}

Transform::Transform(const Transform& rhs):
	Component(rhs),
	_world_matrix(rhs._world_matrix),
	_transform_state()
{
}

auto Transform::GetWorldMatrix() const -> const _matrix&
{
	return _world_matrix;
}

auto Transform::GetWorldMatrixInverse() const -> _matrix
{
	_matrix worldMatrixInverse;
	return *D3DXMatrixInverse(&worldMatrixInverse, nullptr, &_world_matrix);
}

auto Transform::GetState(kState state) -> _float3
{
	return *reinterpret_cast<_float3*>(&_world_matrix.m[static_cast<int32_t>(state)][0]);
}

auto Transform::GetScale() -> _float3
{
	_float3 right, up, look;

	right = GetState(kState::kStateRight);
	up = GetState(kState::kStateUp);
	look = GetState(kState::kStateLook);
	return _float3(	D3DXVec3Length(&right),
					D3DXVec3Length(&up),
					D3DXVec3Length(&look));
}

auto Transform::SetState(kState state, _float3 value) -> void
{
	memcpy(&_world_matrix.m[static_cast<int32_t>(state)][0], value, sizeof(value));
}

auto Transform::SetScale(const float scaleX, const float scaleY, const float scaleZ) -> void
{
	_float3 right, up, look;

	right = GetState(kState::kStateRight);
	up = GetState(kState::kStateUp);
	look = GetState(kState::kStateLook);

	SetState(kState::kStateRight, *D3DXVec3Normalize(&right, &right) * scaleX);
	SetState(kState::kStateUp, *D3DXVec3Normalize(&up, &up) * scaleY);
	SetState(kState::kStateLook, *D3DXVec3Normalize(&look, &look) * scaleZ);
}

void Transform::SetWorldMatrix(const _matrix& matrix)
{
	_world_matrix = matrix;
}

auto Transform::SetMovingState(const bool isMoving) -> void
{
	_is_moving = isMoving;
}

auto Transform::GoStraight(const double timeDelta) -> void
{
	_float3 look = GetState(kState::kStateLook);
	D3DXVec3Normalize(&look, &look);

	_float3 pos = GetState(kState::kStatePosition);
	pos += look * static_cast<float>(_transform_state.speed_per_sec * timeDelta);

	SetState(kState::kStatePosition, pos);
}

auto Transform::BackStraight(const double timeDelta) -> void
{
	_float3 look = GetState(kState::kStateLook);
	D3DXVec3Normalize(&look, &look);

	_float3 pos = GetState(kState::kStatePosition);
	pos -= look * static_cast<float>(_transform_state.speed_per_sec * timeDelta);

	SetState(kState::kStatePosition, pos);
}

auto Transform::WalkLeft(const double timeDelta) -> void
{
	_float3	right = GetState(kState::kStateRight);
	D3DXVec3Normalize(&right, &right);

	_float3	position = GetState(kState::kStatePosition);
	position -= right * static_cast<float>(_transform_state.speed_per_sec * timeDelta);

	SetState(kState::kStatePosition, position);
}

auto Transform::WalkRight(const double timeDelta) -> void
{
	_float3	right = GetState(kState::kStateRight);
	D3DXVec3Normalize(&right, &right);

	_float3	position = GetState(kState::kStatePosition);
	position += right * static_cast<float>(_transform_state.speed_per_sec * timeDelta);

	SetState(kState::kStatePosition, position);
}

auto Transform::RotationAxis(const _float3 axis, const double timeDelta) -> void
{
	_float3	right, up, look;

	right = GetState(kState::kStateRight);
	up = GetState(kState::kStateUp);
	look = GetState(kState::kStateLook);

	_matrix	rotationMatrix;

	D3DXMatrixRotationAxis(&rotationMatrix, &axis, static_cast<float>(_transform_state.speed_per_sec * timeDelta));

	SetState(kState::kStateRight, *D3DXVec3TransformNormal(&right, &right, &rotationMatrix));
	SetState(kState::kStateUp, *D3DXVec3TransformNormal(&up, &up, &rotationMatrix));
	SetState(kState::kStateLook, *D3DXVec3TransformNormal(&look, &look, &rotationMatrix));
}

void Transform::RotationAxis(float radian, _float3 axis)
{
	_float3			right, up, look;

	right = GetState(kState::kStateRight);
	up = GetState(kState::kStateUp);
	look = GetState(kState::kStateLook);

	_matrix			rotationMatrix;
	D3DXMatrixRotationAxis(&rotationMatrix, &axis, radian);

	SetState(kState::kStateRight, *D3DXVec3TransformNormal(&right, &right, &rotationMatrix));
	SetState(kState::kStateUp, *D3DXVec3TransformNormal(&up, &up, &rotationMatrix));
	SetState(kState::kStateLook, *D3DXVec3TransformNormal(&look, &look, &rotationMatrix));
}

auto Transform::RotationAxis(_float3 axis, double timeDelta, float perSecRadian) -> void
{
	_float3	right, up, look;

	right = GetState(kState::kStateRight);
	up = GetState(kState::kStateUp);
	look = GetState(kState::kStateLook);

	_matrix	rotationMatrix;

	D3DXMatrixRotationAxis(&rotationMatrix, &axis, static_cast<float>(perSecRadian * timeDelta));

	SetState(kState::kStateRight, *D3DXVec3TransformNormal(&right, &right, &rotationMatrix));
	SetState(kState::kStateUp, *D3DXVec3TransformNormal(&up, &up, &rotationMatrix));
	SetState(kState::kStateLook, *D3DXVec3TransformNormal(&look, &look, &rotationMatrix));
}

auto Transform::SetUpRotation(_float3 axis, float radian) -> void
{
	_float3	right(1.f, 0.f, 0.f), up(0.f, 1.f, 0.f), look(0.f, 0.f, 1.f);

	const _float3	scale = GetScale();

	right *= scale.x;
	up *= scale.y;
	look *= scale.z;

	_matrix			rotationMatrix;

	D3DXMatrixRotationAxis(&rotationMatrix, &axis, radian);

	SetState(kState::kStateRight, *D3DXVec3TransformNormal(&right, &right, &rotationMatrix));
	SetState(kState::kStateUp, *D3DXVec3TransformNormal(&up, &up, &rotationMatrix));
	SetState(kState::kStateLook, *D3DXVec3TransformNormal(&look, &look, &rotationMatrix));
}

auto Transform::ChaseTarget(const std::shared_ptr<Transform>& targetTransform, const double timeDelta) -> void
{
	_float3 position = GetState(kState::kStatePosition);
		    
	_float3 dir = targetTransform->GetState(kState::kStatePosition) - position;
	const float distance = D3DXVec3Length(&dir);
	D3DXVec3Normalize(&dir, &dir);

	if (distance >= 0.5f)
	{
		position += dir * static_cast<float>(_transform_state.speed_per_sec * timeDelta);
	}

	SetState(kState::kStatePosition, position);

	LookAtTarget(targetTransform);
}

auto Transform::ChaseTarget(_float3 vTargetPos, double timeDelta) -> void
{
	if (false == _is_moving)
	{
		return;
	}

	_float3 position = GetState(kState::kStatePosition);
	_float3	dir = vTargetPos - position;
	const float	distance = D3DXVec3Length(&dir);
	D3DXVec3Normalize(&dir, &dir);

	if (distance >= 0.5f)
	{
		position += dir * static_cast<float>(_transform_state.speed_per_sec * timeDelta);
	}
	else
	{
		_is_moving = false;
	}

	/* 사각형을 구성하는 정점(4개)의 위치에 갱신된 위치정보를 전달해주낟. */
	SetState(kState::kStatePosition, position);

	LookAtTarget(vTargetPos);
}

auto Transform::LookAtTarget(const std::shared_ptr<Transform>& targetTransform) -> void
{
	const _float3 targetPos = targetTransform->GetState(kState::kStatePosition);
	const _float3 position = GetState(kState::kStatePosition);


	_float3	look = targetPos - position;
	D3DXVec3Normalize(&look, &look);
	look = look * GetScale().z;

	_float3	right;
	const _float3 up = GetState(kState::kStateUp);
	D3DXVec3Cross(&right, &up, &look);
	D3DXVec3Normalize(&right, &right);
	right = right * GetScale().x;

	SetState(kState::kStateLook, look);
	SetState(kState::kStateRight, right);
}

auto Transform::LookAtTarget(const _float3 targetPos) -> void
{
	const _float3 position = GetState(kState::kStatePosition);

	_float3	look = targetPos - position;
	D3DXVec3Normalize(&look, &look);
	look = look * GetScale().z;

	_float3	right;
	const _float3 up = GetState(kState::kStateUp);
	D3DXVec3Cross(&right, &up, &look);
	D3DXVec3Normalize(&right, &right);
	right = right * GetScale().x;

	SetState(kState::kStateLook, look);
	SetState(kState::kStateRight, right);
}

auto Transform::StandOnTerrain(std::shared_ptr<ViBufferTerrain> viBuffer, const _matrix* pTerrainWorldMatrix) -> void
{
	//TODO : 지형위에올라가는것 구현해야함
}

auto Transform::RemoveRotation() -> void
{
	const auto scale = GetScale();
	SetState(kState::kStateRight, _float3(1.f, 0.f, 0.f) * scale.x);
	SetState(kState::kStateUp, _float3(0.f, 1.f, 0.f) * scale.y);
	SetState(kState::kStateLook, _float3(0.f, 0.f, 1.f) * scale.z);
}

auto Transform::NativeConstructPrototype()->HRESULT
{
	D3DXMatrixIdentity(&_world_matrix);
	return S_OK;
}

auto Transform::NativeConstruct(void* arg)->HRESULT
{
	if (nullptr != arg)
		memcpy(&_transform_state, arg, sizeof(TransformDesc));

	return S_OK;
}

auto Transform::OrbitRotation(std::shared_ptr<Transform> targer, _float3 axis, double timeDelta) -> _float3
{
	_matrix RotationMatrix;
	_float3 vComputeAngle;
	constexpr double limitAngle = 14.5;

	const float angle = static_cast<float>(_transform_state.rotation_per_sec) * static_cast<float>(timeDelta);

	D3DXMatrixRotationAxis(&RotationMatrix, &axis, angle);

	D3DXVec3TransformCoord(&vComputeAngle, &_orbit_angle, &RotationMatrix);
	_orbit_angle = vComputeAngle;
	return _orbit_angle;
}

auto Transform::Create(const ComPtr<IDirect3DDevice9>& graphicDevice) -> std::shared_ptr<Transform>
{
	auto instance = std::make_shared<Transform>(graphicDevice);

	if (FAILED(instance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Transform");
		return nullptr;
	}
	return instance;
}

auto Transform::Clone(void* arg) -> std::shared_ptr<Component>
{
	auto instance = std::make_shared<Transform>(*this);

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Transform");
		return nullptr;
	}
	return instance;
}

