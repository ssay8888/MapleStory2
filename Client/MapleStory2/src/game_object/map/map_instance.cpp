#include "c_pch.h"
#include "map_instance.h"

#include "cube/map_object.h"
#include "src/game_object/monster/monster.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_objects/manager/object_manager.h"

MapInstance::MapInstance(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

auto MapInstance::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto MapInstance::NativeConstruct(void* arg) -> HRESULT
{
	return GameObject::NativeConstruct(arg);
}

auto MapInstance::Tick(double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto MapInstance::LateTick(double timeDelta) -> HRESULT
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderAlpha, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

auto MapInstance::Render() -> HRESULT
{
	if (_shader_com)
	{
		auto result = _shader_com->BeginShader(0);
		for (auto& mesh : _map_objects)
		{
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
	if (object)
	{
		_map_objects.push_back(object);
	}


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

auto MapInstance::AddMonster(int64_t objectId, std::shared_ptr<Monster> monster) -> void
{
	_monsters.emplace(objectId, monster);
}

auto MapInstance::RemoveMonster(const int64_t objectId) -> void
{
	_monsters.erase(objectId);
}

auto MapInstance::FindMonster(const int64_t objectId) -> std::shared_ptr<Monster>
{
	auto iterator = _monsters.find(objectId);
	if (iterator != _monsters.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto MapInstance::CollisionMonsters(const std::shared_ptr<Collider> collider) -> std::vector<std::shared_ptr<Monster>>
{
	std::vector<std::shared_ptr<Monster>> monsters;
	for (const auto& monster : _monsters)
	{
		if (monster.second->GetMonsterColliderAabb()->CollisionAabb(collider))
		{
			monsters.push_back(monster.second);
		}
	}
	return monsters;
}

auto MapInstance::FindRangeCellObject(const std::shared_ptr<Collider>& targetCollider)->std::vector<std::shared_ptr<MapObject>>
{
	std::vector<std::shared_ptr<MapObject>> objects;
	for (auto& object : _map_objects)
	{
		if (object->GetCollider())
		{
			if (object->GetCollider()->CollisionAabb(targetCollider))
			{
				objects.push_back(object);
			}
		}
	}
	return objects;
}
