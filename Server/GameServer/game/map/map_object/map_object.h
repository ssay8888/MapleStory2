#pragma once
#include "game/map/map_manager.h"
class Transform;
class MeshStatic;
class Collider;

class MapObject abstract : std::enable_shared_from_this<MapObject>
{
public:
	MapObject() = default;
	virtual ~MapObject() = default;

public:
	virtual	auto Tick()->void = 0;
	virtual	auto LateTick()->void = 0;


private:
	std::shared_ptr<Transform>		_transform_com = nullptr;
	std::shared_ptr<MeshStatic>		_mesh_com = nullptr;
	std::shared_ptr<Collider>		_aabb_com = nullptr;
	float							_scale = 1;
};

