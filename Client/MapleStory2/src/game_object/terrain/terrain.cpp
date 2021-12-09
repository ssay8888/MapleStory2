#include "c_pch.h"
#include "terrain.h"

#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/components/vi_buffer/vi_buffer_terrain/vi_buffer_terrain.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"

Terrain::Terrain(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT Terrain::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Terrain::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
		return E_FAIL;
	return GameObject::NativeConstruct(arg);
}

int32_t Terrain::Tick(const double timeDelta)
{

	return GameObject::Tick(timeDelta);
}

int32_t Terrain::LateTick(const double timeDelta)
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT Terrain::Render()
{
	const auto view = PipeLine::GetInstance().GetTransform(D3DTS_VIEW);
	const auto proj = PipeLine::GetInstance().GetTransform(D3DTS_PROJECTION);
	_graphic_device->SetTransform(D3DTS_WORLD, &_transform_com->GetWorldMatrix());
	_graphic_device->SetTransform(D3DTS_VIEW, &view);
	_graphic_device->SetTransform(D3DTS_PROJECTION, &proj);

	auto result = E_FAIL;
	result = _shader_com->SetUpConstantTable("g_WorldMatrix", _transform_com->GetWorldMatrix(), sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &view, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &proj, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseSourTexture", _texture_com, 0);
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseDestTexture", _texture_com, 1);
	result = _shader_com->SetUpTextureConstantTable("g_FilterTexture", _filter_com);

	auto bushPos = _float4(7.f, 0.f, 7.f, 1.f);
	result = _shader_com->SetUpConstantTable("g_vBrushPos", &bushPos, sizeof(_float4));
	constexpr float		range = 7.f;
	result = _shader_com->SetUpConstantTable("g_fRange", &range, sizeof(range));

	const D3DLIGHT9	lightDesc = LightManager::GetInstance().GetLightDesc();
	const auto light = _float4(lightDesc.Direction, 0.f);
	result = _shader_com->SetUpConstantTable("g_vLightDir", &light, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightDiffuse", &lightDesc.Diffuse, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightAmbient", &lightDesc.Ambient, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightSpecular", &lightDesc.Specular, sizeof(_float4));

	const auto campos= _float4(PipeLine::GetInstance().GetCamPosition(), 0);
	result = _shader_com->SetUpConstantTable("g_vCamPosition", &campos, sizeof(_float4));


	result = _shader_com->BeginShader(0);

	_vi_buffer_com->RenderViBuffer();

	result = _shader_com->EndShader();
	return GameObject::Render();
}

auto Terrain::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<Terrain>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Terrain");
		return nullptr;
	}
	return pInstance;
}

auto Terrain::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Terrain>
{
	auto pInstance = std::make_shared<Terrain>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Terrain");
		return nullptr;
	}
	return pInstance;
}

auto Terrain::AddComponents()->HRESULT
{
	/* Com_Transform */
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com))))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Terrain"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	/* Com_VIBuffer*/
	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Shader_Terrain"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Filter"), TEXT("Com_Filter"), reinterpret_cast<std::shared_ptr<Component>*>(&_filter_com))))
		return E_FAIL;

	return S_OK;
}
