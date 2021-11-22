#pragma once
#include "game_contents/game_objects/game_object.h"

static std::atomic<int> g_object_id = 1;
class Monster : public GameObject
{
public:
	Monster(float exp);
	~Monster() = default;

	auto GetObjectId() const -> int64;
	auto GetExp() const -> float;
private:
	float _exp;
	int64 _object_id;
};

