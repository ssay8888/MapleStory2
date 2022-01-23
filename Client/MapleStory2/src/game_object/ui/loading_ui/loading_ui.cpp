#include "c_pch.h"
#include "loading_ui.h"

#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

LoadingUi::LoadingUi():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

auto LoadingUi::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto LoadingUi::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

auto LoadingUi::Tick(const double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto LoadingUi::LateTick(const double timeDelta) -> HRESULT
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

auto LoadingUi::Render() -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);

	transformMatrix._11 = g_WinCX;
	transformMatrix._22 = g_WinCY;
	transformMatrix._41 = 0;
	transformMatrix._42 = 0;
	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _loading_ui, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();

	transformMatrix._11 = g_WinCX;
	transformMatrix._22 = 100.f;
	transformMatrix._41 = 0;
	transformMatrix._42 = (g_WinCY >> 1) - 50.f;
	result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_progress, 1);
	_shader_com->Commit();
	_vi_buffer_com->RenderViBuffer();

	transformMatrix._11 = g_WinCX;
	transformMatrix._22 = 100.f;
	transformMatrix._41 = 0;
	transformMatrix._42 = -(g_WinCY >> 1) + 50.f;

	result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_progress, 0);
	_shader_com->Commit();
	_vi_buffer_com->RenderViBuffer();

	result = _shader_com->EndShader();
	return GameObject::Render();
}

auto LoadingUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<LoadingUi>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto LoadingUi::Create() -> std::shared_ptr<LoadingUi>
{
	auto instance = std::make_shared<LoadingUi>();
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

auto LoadingUi::AddComponents() -> HRESULT
{
	if (FAILED(AddComponent(kScene::kSceneStatic, 
		TEXT("Prototype_VIBuffer_Rect"), 
		TEXT("Com_VIBuffer"), 
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Rect",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
	{
		return E_FAIL;
	}
	
	if (FAILED(AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_Texture_Loading_henesys_02"),
		TEXT("Com_Texture_BackGround1"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_loading_ui))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_Texture_Loading_Ui_Side_Bar"),
		TEXT("Com_Texture_BackGround2"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_texture_progress))))
	{
		return E_FAIL;
	}
	return S_OK;
}
