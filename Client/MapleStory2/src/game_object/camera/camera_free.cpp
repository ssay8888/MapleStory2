#include "c_pch.h"
#include "camera_free.h"

#include "src/system/input/input_device.h"
#include "src/utility/components/transform/transform.h"

CameraFree::CameraFree(const ComPtr<IDirect3DDevice9>& device):
	Camera(device)
{
}

HRESULT CameraFree::NativeConstructPrototype()
{
	return Camera::NativeConstructPrototype();
}

HRESULT CameraFree::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
		return E_FAIL;

	Camera::NativeConstruct(arg);


	return S_OK;
}

int32_t CameraFree::Tick(const double timeDelta)
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
		_transform->RotationAxis(_transform->GetState(Transform::kState::kStateRight) , timeDelta * mouseMove * 0.02f);


	return Camera::Tick(timeDelta);
}

int32_t CameraFree::LateTick(const double timeDelta)
{
	return Camera::LateTick(timeDelta);
}

HRESULT CameraFree::Render()
{
	return S_OK;
}

HRESULT CameraFree::AddComponents()
{
	Transform::TransformDesc			transformDesc;
	transformDesc.speed_per_sec = 10.0f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic), 
		TEXT("Prototype_Transform"), 
		TEXT("Com_Transform"), 
		reinterpret_cast<std::shared_ptr<Component>*>(&_transform), 
		&transformDesc)))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto CameraFree::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<CameraFree>
{
	auto instance = std::make_shared<CameraFree>(device);

	if (FAILED(instance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Camera_Free");
		return nullptr;
	}
	return instance;
}

auto CameraFree::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<CameraFree>(*this);

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Camera_Free");
		return nullptr;
	}
	return instance;
}
