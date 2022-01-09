#pragma once
#include "src/utility/components/component.h"

class Shader;
class Transform;

class Collider final : public Component
{
public:
	enum kCollider { kTypeAabb, kTypeObb, kTypeSphere, kNoTargetAabb, kNoTargetObb, kTypeEnd };

public:
	typedef struct TagColliderDesc
	{
		const _matrix*  parent_matrix;
		_float3			scale = _float3(1.f, 1.f, 1.f);
		_float3			radians = _float3(0.f, 0.f, 0.f);
		_float3			init_pos = _float3(0.f, 0.f, 0.f);
	}Colliderdesc;

	typedef struct TagObbDesc
	{
		_float3			center;
		_float3			align_axis[3];
		_float3			center_axis[3];
	}Obbdesc;
public:
	explicit Collider(const ComPtr<IDirect3DDevice9>& graphicDevice);
	virtual ~Collider() = default;

public:
	auto NativeConstructPrototype(kCollider type)->HRESULT;
	auto NativeConstruct(void* arg)->HRESULT;
	auto UpdateCollider()->void;

public:
	auto CollisionAabb(const std::shared_ptr<Collider>& targetCollider)->bool;
	auto CollisionObb(const std::shared_ptr<Collider>& targetCollider)->bool;
	auto CollisionSphere(std::shared_ptr<Collider> targetCollider)->bool;

public:
	auto GetMax()->_float3;
	auto GetMin()->_float3;

	auto GetTransform()const->std::shared_ptr<Transform>;


private:
	Obbdesc ComputeObbDesc(const _float3* points);


#ifdef _DEBUG
public:
	auto RenderDebug()->HRESULT;
#endif

#ifdef _DEBUG
private:
	HRESULT CompileShaderDebug();
#endif

private:
	kCollider					_type = kTypeEnd;
	LPD3DXMESH					_mesh = nullptr;
	bool						_is_collision = false;
	Colliderdesc				_collider_desc;
	_matrix						_original_matrix;
	_matrix						_parent_matrix;
	std::shared_ptr<Transform>	_transform = nullptr;

	_float3				_min, _max;
	_float3				_points[8];
#ifdef _DEBUG
private:
	std::shared_ptr<Shader> _shader = nullptr;
#endif

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& graphicDevice, kCollider type)->std::shared_ptr<Collider>;
	virtual auto Clone(void* arg)->std::shared_ptr<Component>;
};

