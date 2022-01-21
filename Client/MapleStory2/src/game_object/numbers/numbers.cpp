#include "c_pch.h"
#include "numbers.h"

#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/pipe_line/pipe_line.h"

Numbers::Numbers():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

HRESULT Numbers::NativeConstructPrototype()
{
	_start_time = 0;
	_start_pos = 0;
	_end_pos = 0;
	return GameObject::NativeConstructPrototype();
}

HRESULT Numbers::NativeConstruct(void* arg)
{
	_info = *static_cast<NumberInfo*>(arg);
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	if (_info.parent_transform != nullptr)
	{
		auto pos = _info.parent_transform->GetState(Transform::kState::kStatePosition);
		pos.y += 1.f;
		_transform_com->SetState(Transform::kState::kStatePosition, pos);
	}
	else
	{
		_transform_com->SetState(Transform::kState::kStatePosition, _float3(-5.f, 5.f, -8.f));
	}
	_transform_com->SetScale(0.2f, 0.2f, 0.2f);

	int64_t numbers = _info.numbers;
	float count = 0.f;
	while (numbers != 0)
	{
		numbers /= 10;
		count += 0.1f;
	}
	if (count >= 0.2f)
	{
		_start_pos = -(count / 2.f);
		_end_pos = (count / 2.f);
	}
	else
	{
		_start_pos = 0.f;
		_end_pos = 0.f;
	}

	numbers = _info.numbers;
	while (numbers != 0)
	{
		_numbers.push_back(numbers % 10);
		numbers /= 10;
	}
	_start_time = GetTickCount64();
	return GameObject::NativeConstruct(arg);
}

HRESULT Numbers::Tick(const double timeDelta)
{
	if (_start_time + 500 < GetTickCount64())
	{
		_is_dead = true;
	}
	auto pos = _transform_com->GetState(Transform::kState::kStatePosition);
	pos.y += 0.001f;
	_transform_com->SetState(Transform::kState::kStatePosition, pos);
	return GameObject::Tick(timeDelta);
}

HRESULT Numbers::LateTick(const double timeDelta)
{
	_matrix			viewMatrix = PipeLine::GetInstance().GetTransform(D3DTS_VIEW);

	D3DXMatrixInverse(&viewMatrix, nullptr, &viewMatrix);

	_transform_com->SetState(Transform::kState::kStateUp, *reinterpret_cast<_float3*>(&viewMatrix.m[1][0]));
	_transform_com->SetState(Transform::kState::kStateLook, *reinterpret_cast<_float3*>(&viewMatrix.m[2][0]));
	_transform_com->SetScale(0.15f, 0.15f, 0.15f);

	ComputeViewZ(_transform_com);

	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT Numbers::Render()
{
	auto& pipeline = PipeLine::GetInstance();
	const auto view = pipeline.GetTransform(D3DTS_VIEW);
	const auto project = pipeline.GetTransform(D3DTS_PROJECTION);

	auto result = _shader_com->SetUpConstantTable("g_ViewMatrix", &view, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &project, sizeof(_matrix));

	result = _shader_com->BeginShader(0);

	auto oldPos = _transform_com->GetState(Transform::kState::kStatePosition);
	auto pos = _transform_com->GetState(Transform::kState::kStatePosition);
	pos.x += _start_pos;
	for (auto iterator = _numbers.rbegin(); iterator < _numbers.rend(); ++iterator)
	{
		_transform_com->SetState(Transform::kState::kStatePosition, pos);
		result = _shader_com->SetUpConstantTable("g_WorldMatrix", _transform_com->GetWorldMatrix(), sizeof(_matrix));

		pos.x += 0.1f;
		if (_info.is_player_attack)
		{
			result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _player_attack_numbers, *iterator);
		}
		else
		{
			result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _damage_numbers, *iterator);
		}
		_shader_com->Commit();
		_vi_buffer_com->RenderViBuffer();
	}

	result = _shader_com->EndShader();
	_transform_com->SetState(Transform::kState::kStatePosition, oldPos);
	return GameObject::Render();
}

auto Numbers::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<Numbers>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto Numbers::Create() -> std::shared_ptr<Numbers>
{
	auto instance = std::make_shared<Numbers>();
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

auto Numbers::AddComponents() -> HRESULT
{

	Transform::TransformDesc		TransformDesc;
	TransformDesc.speed_per_sec = 1.f;
	TransformDesc.rotation_per_sec = D3DXToRadian(90.f);


	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Damage_Number"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_damage_numbers))))
	{
		return E_FAIL;
	}

	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Player_Number"), TEXT("Com_Texture_2"), reinterpret_cast<std::shared_ptr<Component>*>(&_player_attack_numbers))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &TransformDesc)))
	{
		return E_FAIL;
	}
	
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Shader_Rect"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
	{
		return E_FAIL;
	}

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	return S_OK;
}
