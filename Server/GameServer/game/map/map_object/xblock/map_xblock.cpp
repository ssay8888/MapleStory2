#include "game_server_pch.h"
#include "map_xblock.h"

#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"
#include "src/utils/file_utils.h"

auto MapXblock::Create(MapManager::MapEntity& entity) -> std::shared_ptr<MapXblock>
{
	auto instance = std::make_shared<MapXblock>();
	if (FAILED(instance->NativeConstruct(entity)))
	{
		return nullptr;
	}
	return instance;
}

auto MapXblock::Tick() -> void
{
}

auto MapXblock::LateTick() -> void
{
}

auto MapXblock::NativeConstruct(MapManager::MapEntity& entity) -> HRESULT
{
	_transform_com = Transform::Create(nullptr);
	if (!entity.propertise.empty())
	{
		auto en = entity.propertise.find("Scale");
		if (en != entity.propertise.end())
		{
			_scale = en->second.x;
		}
		en = entity.propertise.find("Position");
		if (en != entity.propertise.end())
		{
			_transform_com->SetState(Transform::kState::kStatePosition, (en->second / 150.f * 0.58f));
		}
		en = entity.propertise.find("Rotation");
		if (en != entity.propertise.end())
		{
			if (en->second.x != 0)
			{
				_float3 axis{ 0, 0,  -1 };
				_transform_com->SetUpRotation(axis, D3DXToRadian(en->second.x));
			}
			if (en->second.z != 0)
			{
				_float3 axis{ -1, 0, 0 };
				_transform_com->SetUpRotation(axis, D3DXToRadian(en->second.z));
			}
			if (en->second.y != 0)
			{
				_float3 axis{ 0, -1, 0 };
				_transform_com->SetUpRotation(axis, D3DXToRadian(en->second.y));

			}
		}
	}


	Collider::TagColliderDesc		ColliderDesc;
	ColliderDesc.parent_matrix = &_transform_com->GetWorldMatrix();
	ColliderDesc.scale = _float3(0.59f, 0.59f, 0.59f);
	ColliderDesc.init_pos = _float3(0.f, 0.295f, 0.f);

	if (!entity.propertise.empty())
	{
		auto en = entity.propertise.find("Rotation");
		if (en != entity.propertise.end())
		{
			if (en->second.x != 0)
			{
				ColliderDesc.radians = _float3(D3DXToRadian(en->second.x), 0.f, 0.f);
			}
			if (en->second.z != 0)
			{
				ColliderDesc.radians = _float3(0.f, D3DXToRadian(en->second.z), 0.f);
			}
			if (en->second.y != 0)
			{
				ColliderDesc.radians = _float3(0.f, 0.f, D3DXToRadian(en->second.y));
			}
		}
	}
	_aabb_com = Collider::Create(nullptr, Collider::kTypeAabb);
	_aabb_com->NativeConstruct(&ColliderDesc);

	_transform_com->SetScale((_scale * 0.01f), (_scale * 0.01f), (_scale * 0.01f));

	return S_OK;
}
