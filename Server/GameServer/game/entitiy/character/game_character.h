#pragma once
#include "game/entitiy/game_entity.h"

class GameCharacter final : public GameEntity
{
public:
	explicit GameCharacter(int64_t characterId);
	~GameCharacter() = default;

public:
	static auto Create(int64_t characterId)->std::shared_ptr<GameCharacter>;

public:
	auto GetCharacterId()const->int64_t;
	auto GetAccountId()const->int32_t;
	auto GetGender()const->int32_t;
	auto GetFaceId()const->int32_t;
	auto GetName()const->std::wstring;

private:
	auto NativeConstruct()->HRESULT;

private:
	int64_t			_character_id;
	int32_t			_account_id;
	int32_t			_gender;
	int32_t			_face_id;
	std::wstring	_name;

};

