#pragma once
#include "game/map/map_object/map_object.h"

class MapXblock final : public MapObject
{
public:
	MapXblock() = default;
	virtual ~MapXblock() = default;

public:
	static auto Create(MapManager::MapEntity& entity)->std::shared_ptr<MapXblock>;
	virtual	auto Tick()->void;
	virtual	auto LateTick()->void;

public:
	auto GetCollider()const->std::shared_ptr<Collider>;
	auto GetTransform()const->std::shared_ptr<Transform>;

private:
	auto NativeConstruct(MapManager::MapEntity& entity)->HRESULT;

private:
	std::shared_ptr<Transform>		_transform_com = nullptr;
	std::shared_ptr<MeshStatic>		_mesh_com = nullptr;
	std::shared_ptr<Collider>		_aabb_com = nullptr;
	float							_scale = 1;
};

