#include "c_pch.h"
#include "map_instance.h"

#include "cube/map_object.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_objects/manager/object_manager.h"

MapInstance::MapInstance(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT MapInstance::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT MapInstance::NativeConstruct(void* arg)
{
	return GameObject::NativeConstruct(arg);
}

int32_t MapInstance::Tick(double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t MapInstance::LateTick(double timeDelta)
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderAlpha, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT MapInstance::Render()
{
	if (_shader_com)
	{
		auto result = _shader_com->BeginShader(0);
		for (auto& mesh : _map_objects)
		{
			mesh->Tick(0);
			SetUpConstantTable(mesh->GetTransform());
			mesh->Render(_shader_com);
		}
		result = _shader_com->EndShader();
	}
	return GameObject::Render();
}

auto MapInstance::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto MapInstance::AddMapObject(MapParser::MapEntity modelName) -> bool
{
	auto& objectManager = ObjectManager::GetInstance();
	const auto object = MapObject::Create(&modelName);
	_map_objects.push_back(object);


	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Shader_Mesh"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
	{
		return false;
	}

	return true;
}

auto MapInstance::SetUpConstantTable(const std::shared_ptr<Transform> transformCom) const->HRESULT
{
	auto& pipeline = PipeLine::GetInstance();
	const auto view = pipeline.GetTransform(D3DTS_VIEW);
	const auto project = pipeline.GetTransform(D3DTS_PROJECTION);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", transformCom->GetWorldMatrix(), sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &view, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &project, sizeof(_matrix));

	const D3DLIGHT9	lightDesc = LightManager::GetInstance().GetLightDesc(0);
	const auto lightDir = _float4(lightDesc.Direction, 0.f);
	result = _shader_com->SetUpConstantTable("g_vLightDir", &lightDir, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightDiffuse", &lightDesc.Diffuse, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightAmbient", &lightDesc.Ambient, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightSpecular", &lightDesc.Specular, sizeof(_float4));
	const auto camPos = _float4(pipeline.GetCamPosition(), 1.f);
	result = _shader_com->SetUpConstantTable("g_vCamPosition", &camPos, sizeof(_float4));
	return S_OK;
}

auto MapInstance::FindRangeCellObject(const std::shared_ptr<Collider>& targetCollider)->std::vector<std::shared_ptr<MapObject>>
{
	std::vector<std::shared_ptr<MapObject>> objects;
	for (auto& object : _map_objects)
	{
		if (object->GetCollider()->CollisionAabb(targetCollider))
		{
			objects.push_back(object);
		}
	}
	return objects;
}
