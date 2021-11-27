#pragma once
#include "src/utility/components/component.h"

class ViBufferTerrain;

class Transform final : public Component
{
public:
	enum class kState
	{
		kStateRight,
		kStateUp,
		kStateLook,
		kStatePosition,
		kStateEnd
	};

	typedef struct TransformDesc
	{
		double speed_per_sec;
		double rotation_per_sec;
	} TransformDesc;

public:
	Transform(const ComPtr<IDirect3DDevice9>& graphicDevice);
	Transform(const Transform& rhs);
	virtual ~Transform() = default;

public:
	auto GetWorldMatrix() const->const _matrix&;
	auto GetWorldMatrixInverse() const->_matrix;
	auto GetState(kState state)->_float3;
	auto GetScale()->_float3;

public:
	auto SetState(kState state, _float3 value)->void;
	auto SetScale(float scaleX, float scaleY, float scaleZ)->void;
	auto SetMovingState(bool isMoving)->void;

public:
	auto GoStraight(const double timeDelta)->void;
	auto BackStraight(const double timeDelta)->void;
	auto WalkLeft(const double timeDelta)->void;
	auto WalkRight(const double timeDelta)->void;
	auto RotationAxis(_float3 axis, double timeDelta)->void;
	auto SetUpRotation(_float3 axis, float radian)->void;
	auto ChaseTarget(const std::shared_ptr<Transform>& targetTransform, double timeDelta)->void;
	auto ChaseTarget(_float3 vTargetPos, double timeDelta)->void;
	auto LookAtTarget(const std::shared_ptr<Transform>& targetTransform)->void;
	auto LookAtTarget(_float3 targetPos)->void;
	auto StandOnTerrain(class std::shared_ptr<ViBufferTerrain> viBuffer, const _matrix* pTerrainWorldMatrix)->void;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	static auto Create(const ComPtr<IDirect3DDevice9>& graphicDevice)->std::shared_ptr<Transform>;
	auto Clone(void* arg)->std::shared_ptr<Component> override;

private:
	/* 객체의 상태를 표현한다. */
	_matrix			_world_matrix;
	TransformDesc	_transform_state;

private:
	bool			_is_moving = false;
};

