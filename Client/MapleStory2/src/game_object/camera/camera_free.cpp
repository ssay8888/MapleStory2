#include "pch.h"
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
	if (InputDevice::GetInstance().GetDirectKeyState(DIK_W) & 0x80)
	{
		_transform->GoStraight(timeDelta);
	}

	if (GetKeyState('S') < 0)
	{
		_transform->BackStraight(timeDelta);
	}

	if (GetKeyState('A') < 0)
	{
		_transform->WalkLeft(timeDelta);
	}

	if (GetKeyState('D') < 0)
	{
		_transform->WalkRight(timeDelta);
	}
	
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
	transformDesc.rotation_per_sec = D3DXToRadian(90.0f);

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
