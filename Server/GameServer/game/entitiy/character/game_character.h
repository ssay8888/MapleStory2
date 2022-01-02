#pragma once
#include "game/entitiy/game_entity.h"

class Transform;

class GameCharacter final : public GameEntity
{
public:
	explicit GameCharacter(int64_t characterId);
	virtual ~GameCharacter() = default;

public:
	static auto Create(int64_t characterId)->std::shared_ptr<GameCharacter>;

public:
	auto GetCharacterId()const->int64_t;
	auto GetAccountId()const->int32_t;
	auto GetGender()const->int32_t;
	auto GetFaceId()const->int32_t;
	auto GetName()const->std::wstring;
	auto GetMapId()const->int32_t;
	auto GetSpawnPoint()const->int32_t;
	auto GetTransForm()const->std::shared_ptr<Transform>;
private:
	auto NativeConstruct()->HRESULT;

private:
	int64_t			_character_id;
	int32_t			_account_id;
	int32_t			_gender;
	int32_t			_face_id;
	int32_t			_map_id;
	int32_t			_spawn_point;
	std::wstring	_name;
	std::shared_ptr<Transform> _transform;
};

