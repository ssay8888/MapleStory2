#include "pch.h"
#include "collider.h"

#include "src/utility/components/shader/shader.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/pipe_line/pipe_line.h"

Collider::Collider(const ComPtr<IDirect3DDevice9>& graphicDevice):
	Component(graphicDevice), _collider_desc()
{
}

auto Collider::NativeConstructPrototype(const kCollider type) -> HRESULT
{
	if (FAILED(__super::NativeConstructPrototype()))
		return E_FAIL;


#ifdef _DEBUG
	if (FAILED(CompileShaderDebug()))
		return E_FAIL;
#endif

	_type = type;

	HRESULT			hr = 0;

	switch (_type)
	{
	case kNoTargetAabb:
	case kTypeAabb:
		hr = D3DXCreateBox(_graphic_device.Get(), 1.f, 1.f, 1.f, &_mesh, nullptr);

		_min = _float3(-0.5f, -0.5f, -0.5f);
		_max = _float3(0.5f, 0.5f, 0.5f);
		break;

	case kNoTargetObb:
	case kTypeObb:
		hr = D3DXCreateBox(_graphic_device.Get(), 1.f, 1.f, 1.f, &_mesh, nullptr);

		_points[0] = _float3(-0.5f, 0.5f, -0.5f);
		_points[1] = _float3(0.5f, 0.5f, -0.5f);
		_points[2] = _float3(0.5f, -0.5f, -0.5f);
		_points[3] = _float3(-0.5f, -0.5f, -0.5f);

		_points[4] = _float3(-0.5f, 0.5f, 0.5f);
		_points[5] = _float3(0.5f, 0.5f, 0.5f);
		_points[6] = _float3(0.5f, -0.5f, 0.5f);
		_points[7] = _float3(-0.5f, -0.5f, 0.5f);

		break;

	case kTypeSphere:
		hr = D3DXCreateSphere(_graphic_device.Get(), 0.5f, 10, 10, &_mesh, nullptr);
		break;
	case kTypeEnd: break;
	default: ;
	}

	return S_OK;
}

HRESULT Collider::NativeConstruct(void* arg)
{
	if (FAILED(__super::NativeConstruct(arg)))
		return E_FAIL;

	memcpy(&_collider_desc, arg, sizeof(Colliderdesc));

	_transform = Transform::Create(_graphic_device);
	if (nullptr == _transform)
		return E_FAIL;

	_float3		vPosition = _transform->GetState(Transform::kState::kStatePosition);

	_transform->SetState(Transform::kState::kStatePosition, _collider_desc.init_pos);
	_transform->SetScale(_collider_desc.scale.x, _collider_desc.scale.y, _collider_desc.scale.z);
	//_transform->RotationAxis(_collider_desc.radians.x, _float3(-1.f, 0.f, 0.f));
	//_transform->RotationAxis(_collider_desc.radians.y, _float3(0.f,-1.f, 0.f));
	//_transform->RotationAxis(_collider_desc.radians.z, _float3(0.f, 0.f, -1.f));

	if (_collider_desc.radians.x != 0)
	{
		_float3 axis{ 0, 0,  1 };
		_transform->SetUpRotation(axis, D3DXToRadian(_collider_desc.radians.x));
	}
	if (_collider_desc.radians.z != 0)
	{
		_float3 axis{ 1, 0, 0 };
		_transform->SetUpRotation(axis, D3DXToRadian(_collider_desc.radians.z));
	}
	if (_collider_desc.radians.y != 0)
	{
		_float3 axis{ 0, 1, 0 };
		_transform->SetUpRotation(axis, D3DXToRadian(_collider_desc.radians.y));
	}
	_original_matrix = _transform->GetWorldMatrix();
	

	return S_OK;
}

auto Collider::UpdateCollider() -> void
{
	_parent_matrix = *_collider_desc.parent_matrix;
	
	D3DXVec3Normalize(reinterpret_cast<_float3*>(&_parent_matrix.m[0][0]), reinterpret_cast<_float3*>(&_parent_matrix.m[0][0]));
	D3DXVec3Normalize(reinterpret_cast<_float3*>(&_parent_matrix.m[1][0]), reinterpret_cast<_float3*>(&_parent_matrix.m[1][0]));
	D3DXVec3Normalize(reinterpret_cast<_float3*>(&_parent_matrix.m[2][0]), reinterpret_cast<_float3*>(&_parent_matrix.m[2][0]));
	switch (_type)
	{
	case kTypeAabb:
	case kTypeObb:
		_transform->SetWorldMatrix(_original_matrix * _parent_matrix);
		break;

	case kTypeSphere: break;
	case kNoTargetAabb:
	case kNoTargetObb:
		_transform->SetWorldMatrix(_original_matrix * _parent_matrix);
		break;
	case kTypeEnd: break;
	default: ;
	}

	if (_type == kTypeAabb || _type == kNoTargetAabb)
		_transform->RemoveRotation();
}

auto Collider::CollisionAabb(const std::shared_ptr<Collider>& targetCollider) -> bool
{
	_float3		vSourMin, vSourMax;
	_float3		vDestMin, vDestMax;

	const _matrix		transformMatrix = _transform->GetWorldMatrix();

	D3DXVec3TransformCoord(&vSourMin, &_min, &transformMatrix);
	D3DXVec3TransformCoord(&vSourMax, &_max, &transformMatrix);

	D3DXVec3TransformCoord(&vDestMin, &targetCollider->_min, &targetCollider->_transform->GetWorldMatrix());
	D3DXVec3TransformCoord(&vDestMax, &targetCollider->_max, &targetCollider->_transform->GetWorldMatrix());

	/* ³Êºñ·Î °ãÃÆ´Ï? */
	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
	{
		return _is_collision = false;
	}

	/* ³ôÀÌ·Î °ãÃÆ´Ï? */
	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
	{
		return _is_collision = false;
	}

	/* ±íÀÌ·Î °ãÃÆ´Ï? */
	if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
	{
		return _is_collision = false;
	}

	return _is_collision = true;
}

auto Collider::CollisionObb(const std::shared_ptr<Collider>& targetCollider) -> bool
{
	_float3		vSourPoint[8], vDestPoint[8];

	for (int32_t i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&vSourPoint[i], &_points[i], &_transform->GetWorldMatrix());
		D3DXVec3TransformCoord(&vDestPoint[i], &targetCollider->_points[i], &targetCollider->_transform->GetWorldMatrix());
	}

	Obbdesc			OBBDesc[2];

	OBBDesc[0] = ComputeObbDesc(vSourPoint);
	OBBDesc[1] = ComputeObbDesc(vDestPoint);

	float			fDistance[3];

	for (int32_t i = 0; i < 2; ++i)
	{
		for (int32_t j = 0; j < 3; ++j)
		{
			fDistance[0] = static_cast<float>(fabs(D3DXVec3Dot(&OBBDesc[0].center_axis[0], &OBBDesc[i].align_axis[j]))) +
				static_cast<float>(fabs(D3DXVec3Dot(&OBBDesc[0].center_axis[1], &OBBDesc[i].align_axis[j]))) +
				static_cast<float>(fabs(D3DXVec3Dot(&OBBDesc[0].center_axis[2], &OBBDesc[i].align_axis[j])));

			fDistance[1] =static_cast<float>(fabs(D3DXVec3Dot(&OBBDesc[1].center_axis[0], &OBBDesc[i].align_axis[j]))) +
				static_cast<float>(fabs(D3DXVec3Dot(&OBBDesc[1].center_axis[1], &OBBDesc[i].align_axis[j]))) +
				static_cast<float>(fabs(D3DXVec3Dot(&OBBDesc[1].center_axis[2], &OBBDesc[i].align_axis[j])));


			auto sub = (OBBDesc[1].center - OBBDesc[0].center);
			fDistance[2] = static_cast<float>(fabs(D3DXVec3Dot(&sub, &OBBDesc[i].align_axis[j])));

			if (fDistance[2] > fDistance[0] + fDistance[1])
			{
				_is_collision = false;
				return _is_collision;
			}

		}
	}
	_is_collision = true;
	return _is_collision;
}

auto Collider::CollisionSphere(std::shared_ptr<Collider> targetCollider) -> bool
{
	return false;
}

auto Collider::GetMax() -> _float3
{
	_float3		vSourMax;
	const _matrix		transformMatrix = _transform->GetWorldMatrix();
	D3DXVec3TransformCoord(&vSourMax, &_max, &transformMatrix);
	return vSourMax;
}

auto Collider::GetMin() -> _float3
{
	_float3		vSourMin;
	const _matrix		transformMatrix = _transform->GetWorldMatrix();
	D3DXVec3TransformCoord(&vSourMin, &_min, &transformMatrix);
	return vSourMin;
}

Collider::Obbdesc Collider::ComputeObbDesc(const _float3* points)
{
	Obbdesc			OBBDesc;
	ZeroMemory(&OBBDesc, sizeof(Obbdesc));

	OBBDesc.center = (points[5] + points[3]) * 0.5f;
	OBBDesc.align_axis[0] =points[5] - points[4];
	OBBDesc.align_axis[1] =points[5] - points[6];
	OBBDesc.align_axis[2] =points[5] - points[1];

	for (int32_t i = 0; i < 3; ++i)
		D3DXVec3Normalize(&OBBDesc.align_axis[i], &OBBDesc.align_axis[i]);

	OBBDesc.center_axis[0] = (points[5] + points[2]) * 0.5f - OBBDesc.center;
	OBBDesc.center_axis[1] = (points[5] + points[0]) * 0.5f - OBBDesc.center;
	OBBDesc.center_axis[2] = (points[5] + points[7]) * 0.5f - OBBDesc.center;

	return OBBDesc;
}

auto Collider::RenderDebug() -> HRESULT
{
	if (nullptr == _mesh ||
		nullptr == _graphic_device)
		return E_FAIL;

	auto& pipeLine = PipeLine::GetInstance();

	auto result = _shader->SetUpConstantTable("g_WorldMatrix", _transform->GetWorldMatrix(), sizeof(_matrix));
	const auto viewMatrix = pipeLine.GetTransform(D3DTS_VIEW);
	result = _shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	const auto projMatrix = pipeLine.GetTransform(D3DTS_PROJECTION);
	result = _shader->SetUpConstantTable("g_ProjMatrix", &projMatrix, sizeof(_matrix));
	result = _shader->SetUpBool("g_isCollision", _is_collision);
	

	result = _shader->BeginShader(0);

	_mesh->DrawSubset(0);

	result = _shader->EndShader();

	return S_OK;
}

HRESULT Collider::CompileShaderDebug()
{
	_shader = Shader::Create(_graphic_device, TEXT("../../Binary/ShaderFiles/Shader_Collider.hlsl"));
	if (nullptr == _shader)
		return E_FAIL;

	return S_OK;
}

auto Collider::Create(const ComPtr<IDirect3DDevice9>& graphicDevice, kCollider type) -> std::shared_ptr<Collider>
{
	auto instance = std::make_shared<Collider>(graphicDevice);

	if (FAILED(instance->NativeConstructPrototype(type)))
	{
		MSGBOX("Failed to Creating Collider");
		return nullptr;
	}
	return instance;
}

auto Collider::Clone(void* arg) -> std::shared_ptr<Component>
{
	auto instance = std::make_shared<Collider>(*this);

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Collider");
		return nullptr;
	}
	return instance;
}
