#include "c_pch.h"
#include "camera_free.h"

#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/transform/transform.h"

CameraFree::CameraFree(const ComPtr<IDirect3DDevice9>& device):
	Camera(device)
{
}

auto CameraFree::NativeConstructPrototype() -> HRESULT
{
	return Camera::NativeConstructPrototype();
}

auto CameraFree::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(AddComponents()))
		return E_FAIL;

	Camera::NativeConstruct(arg);


	return S_OK;
}

auto CameraFree::Tick(const double timeDelta) -> HRESULT
{
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_W))
	//{
	//	_transform->GoStraight(timeDelta);
	//}

	//if (InputDevice::GetInstance().GetKeyPressing(DIK_S))
	//{
	//	_transform->BackStraight(timeDelta);
	//}

	//if (InputDevice::GetInstance().GetKeyPressing(DIK_A))
	//{
	//	_transform->WalkLeft(timeDelta);
	//}

	//if (InputDevice::GetInstance().GetKeyPressing(DIK_D))
	//{
	//	_transform->WalkRight(timeDelta);
	//}
	/*
	if (InputDevice::GetInstance().GetKeyPressing(DIK_LEFT))
	{
		_transform->RotationAxis(_float3(0.f, 1.f, 0.f), timeDelta * -1 * 0.02f);
	}
	if (InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT))
	{
		_transform->RotationAxis(_float3(0.f, 1.f, 0.f), timeDelta * 1 * 0.02f);
	}

	if (InputDevice::GetInstance().GetKeyPressing(DIK_UP))
	{
		_transform->RotationAxis(_transform->GetState(Transform::kState::kStateRight) , timeDelta * 1 * 0.02f);
	}
	if (InputDevice::GetInstance().GetKeyPressing(DIK_DOWN))
	{
		_transform->RotationAxis(_transform->GetState(Transform::kState::kStateRight) , timeDelta * -1 * 0.02f);
	}


	if (InputDevice::GetInstance().GetKeyDown(DIK_Z))
	{
		_float3 axis{ 0, 1,  0 };
		_transform->RotationAxis(axis, 1, D3DXToRadian(90.f));
	}*/

	//int64_t			mouseMove = 0;

	//mouseMove = InputDevice::GetInstance().GetDirectMouseMoveState(InputDevice::kDirectInMouseState::kX);

	//if (0 != mouseMove)
	//	_transform->RotationAxis(_float3(0.f, 1.f, 0.f), timeDelta * mouseMove * 0.02f);

	//mouseMove = InputDevice::GetInstance().GetDirectMouseMoveState(InputDevice::kDirectInMouseState::kY);

	//if (0 != mouseMove)
	//	_transform->RotationAxis(_transform->GetState(Transform::kState::kStateRight) , timeDelta * mouseMove * 0.02f);

	return Camera::Tick(timeDelta);
}

auto CameraFree::LateTick(const double timeDelta) -> HRESULT
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return Camera::LateTick(timeDelta);
}

auto CameraFree::Render() -> HRESULT
{
	std::wstring str;

	RECT rc = {
		0, 0,
		1000, 1000
	};
	str.append(L"¶ó : ").append(std::to_wstring(_transform->GetState(Transform::kState::kStateRight).x)).append(L" / ")
		.append(std::to_wstring(_transform->GetState(Transform::kState::kStateRight).y)).append(L" / ")
		.append(std::to_wstring(_transform->GetState(Transform::kState::kStateRight).z)).append(L"\r\n");

	str.append(L"¾÷ : ").append(std::to_wstring(_transform->GetState(Transform::kState::kStateUp).x)).append(L" / ")
		.append(std::to_wstring(_transform->GetState(Transform::kState::kStateUp).y)).append(L" / ")
		.append(std::to_wstring(_transform->GetState(Transform::kState::kStateUp).z)).append(L"\r\n");

	str.append(L"·è : ").append(std::to_wstring(_transform->GetState(Transform::kState::kStateLook).x)).append(L" / ")
		.append(std::to_wstring(_transform->GetState(Transform::kState::kStateLook).y)).append(L" / ")
		.append(std::to_wstring(_transform->GetState(Transform::kState::kStateLook).z)).append(L"\r\n");

	str.append(L"Æ÷ : ").append(std::to_wstring(_transform->GetState(Transform::kState::kStatePosition).x)).append(L" / ")
		.append(std::to_wstring(_transform->GetState(Transform::kState::kStatePosition).y)).append(L" / ")
		.append(std::to_wstring(_transform->GetState(Transform::kState::kStatePosition).z)).append(L"\r\n");

	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, str.c_str(), -1, &rc, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	return S_OK;
}

auto CameraFree::AddComponents() -> HRESULT
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
