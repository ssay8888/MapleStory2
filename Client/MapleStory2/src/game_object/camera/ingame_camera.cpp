#include "c_pch.h"
#include "ingame_camera.h"

#include "src/system/input/input_device.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_objects/manager/object_manager.h"

IngameCamera::IngameCamera(const ComPtr<IDirect3DDevice9>& device) :
	Camera(device)
{
}

HRESULT IngameCamera::NativeConstructPrototype()
{
	return Camera::NativeConstructPrototype();
}

HRESULT IngameCamera::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
		return E_FAIL;
	const auto& objectManager = ObjectManager::GetInstance();
	auto target = objectManager.GetComponentPtr(kSceneGamePlay0, L"Layer_Character", L"Com_Transform");
	if (!target)
	{
		return E_FAIL;
	}
	_target_transform_com = std::static_pointer_cast<Transform>(target);
	_transform->LookAtTarget(_target_transform_com);
	Camera::NativeConstruct(arg);
	
	return S_OK;
}

int32_t IngameCamera::Tick(const double timeDelta)
{
	if (_free_camera)
	{
		if (InputDevice::GetInstance().GetKeyPressing(DIK_W))
		{
			_transform->GoStraight(timeDelta);
		}

		if (InputDevice::GetInstance().GetKeyPressing(DIK_S))
		{
			_transform->BackStraight(timeDelta);
		}

		if (InputDevice::GetInstance().GetKeyPressing(DIK_A))
		{
			_transform->WalkLeft(timeDelta);
		}

		if (InputDevice::GetInstance().GetKeyPressing(DIK_D))
		{
			_transform->WalkRight(timeDelta);
		}

		int64_t			mouseMove = 0;

		mouseMove = InputDevice::GetInstance().GetDirectMouseMoveState(InputDevice::kDirectInMouseState::kX);

		if (0 != mouseMove)
			_transform->RotationAxis(_float3(0.f, 1.f, 0.f), timeDelta * mouseMove * 0.02f);

		mouseMove = InputDevice::GetInstance().GetDirectMouseMoveState(InputDevice::kDirectInMouseState::kY);

		if (0 != mouseMove)
			_transform->RotationAxis(_transform->GetState(Transform::kState::kStateRight), timeDelta * mouseMove * 0.02f);
	}

	if (InputDevice::GetInstance().GetKeyDown(DIK_F4))
	{
		_free_camera = !_free_camera;
	}
	return Camera::Tick(timeDelta);
}

int32_t IngameCamera::LateTick(const double timeDelta)
{
	if (!_free_camera)
	{
		auto targetPos = _target_transform_com->GetState(Transform::kState::kStatePosition);
		targetPos.y += 1.854213f;
		targetPos.z += -1.5f;
		_transform->SetState(Transform::kState::kStatePosition,
			targetPos);
	}


	return Camera::LateTick(timeDelta);
}

HRESULT IngameCamera::Render()
{
	return Camera::Render();
}

HRESULT IngameCamera::AddComponents()
{
	Transform::TransformDesc			transformDesc;
	transformDesc.speed_per_sec = 10.0f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_Transform"),
		TEXT("Com_Transform"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_transform),
		&transformDesc)))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto IngameCamera::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<IngameCamera>
{
	auto instance = std::make_shared<IngameCamera>(device);

	if (FAILED(instance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating IngameCamera");
		return nullptr;
	}
	return instance;
}

auto IngameCamera::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<IngameCamera>(*this);

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Camera_Free");
		return nullptr;
	}
	return instance;
}
