#pragma once
#include "game/entitiy/game_entity.h"

class GameCharacter : public GameEntity
{
public:
	explicit GameCharacter(int64_t characterId);
	~GameCharacter() = default;

private:
	auto NativeConstruct()->HRESULT;

private:
	int64_t _character_id;
};

