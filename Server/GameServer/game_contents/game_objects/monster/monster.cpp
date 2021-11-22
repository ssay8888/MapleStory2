#include "pch.h"
#include "monster.h"

Monster::Monster(float exp) :
	_exp(exp),
	_object_id(g_object_id++)
{
}

auto Monster::GetObjectId() const -> int64
{
	return _object_id;
}

auto Monster::GetExp() const -> float
{
	return _exp;
}
