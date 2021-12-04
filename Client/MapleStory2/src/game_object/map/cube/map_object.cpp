#include "c_pch.h"
#include "map_object.h"

#include "src/common/xml/map_parser.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/meshes/static/mesh_static.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/transform/transform.h"
#include "src/utils/file_utils.h"

MapObject::MapObject() : GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

auto MapObject::NativeConstructPrototype()->HRESULT
{
	return S_OK;
}

auto MapObject::NativeConstruct(void* arg)->HRESULT
{
	if (arg)
	{
		auto entity = static_cast<MapParser::MapEntity*>(arg);
		AddComponents(*entity);
	}
	return S_OK;
}

auto MapObject::Tick(double timeDelta)-> int32_t
{
	return S_OK;
}

auto MapObject::LateTick(double timeDelta)->int32_t
{
	return S_OK;
}

auto MapObject::Render()->HRESULT
{
	return GameObject::Render();
}

auto MapObject::Render(const std::shared_ptr<Shader>& shaderCom)->HRESULT
{
	if (_mesh_com)
	{
		if (FAILED(_mesh_com->SetUpTextureOnShader(shaderCom, "g_DiffuseTexture", MeshMaterialTexture::kType::kTypeDiffuse, 0)))
		{

		}
		shaderCom->Commit();
		if (FAILED(_mesh_com->Render(0)))
			return E_FAIL;
	}

	return S_OK;
}

auto MapObject::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto MapObject::Create(void* arg) -> std::shared_ptr<MapObject>
{
	auto instance = std::make_shared<MapObject>();

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Creating MapObject");
		return nullptr;
	}
	return instance;
}

auto MapObject::AddComponents(MapParser::MapEntity& entity) -> HRESULT
{
	Transform::TransformDesc		transformDesc;
	transformDesc.speed_per_sec = 5.0f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);

	if (FAILED(AddComponent(static_cast<int32_t>(kScene::kSceneStatic), TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &transformDesc)))
		return E_FAIL;
	if (entity.propertise.size() > 0 && entity.propertise[0].property.size() > 0)
	{
		auto en = entity.propertise[0].property.find("Position");
		if (en != entity.propertise[0].property.end())
		{
			_transform_com->SetState(Transform::kState::kStatePosition, en->second / 150.f * 0.58f);
		}

		if (entity.propertise.size() >= 2 && entity.propertise[1].property.size() > 0)
		{
			auto en2 = entity.propertise[1].property.find("Rotation");
			if (en2 != entity.propertise[1].property.end())
			{
				if (en2->second.x != 0)
				{
					_float3 axis{ 0, 0,  -1};
					_transform_com->SetUpRotation(axis, D3DXToRadian(en2->second.x ));
				}
				if (en2->second.z != 0)
				{
					_float3 axis{ -1, 0, 0 };
					_transform_com->SetUpRotation(axis, D3DXToRadian(en2->second.z));
				}
				if (en2->second.y != 0)
				{
					_float3 axis{ 0, -1, 0};
					_transform_com->SetUpRotation(axis, D3DXToRadian(en2->second.y));
					
				}
			}
		}
		//_transform_com->SetUpRotation(_float3{ 0, 1, 0 }, D3DXToRadian(270.f));

	}
	_transform_com->SetScale(0.01f, 0.01f, 0.01f);

	if (FAILED(AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), std::wstring(L"Prototype_Mesh_Cube_").append(FileUtils::ConvertCtoW(entity.modelName.c_str())), TEXT("Com_Mesh"), reinterpret_cast<std::shared_ptr<Component>*>(&_mesh_com))))
		return E_FAIL;

	return S_OK;
}

auto MapObject::GetTransform() -> std::shared_ptr<Transform>&
{
	return _transform_com;
}
