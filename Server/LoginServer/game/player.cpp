#include "pch.h"
#include "player.h"

#include "equipped/equipped.h"


Player::Player(const int64_t characterId) :
	_gender(0),
    _character_id(characterId),
    _face_id(0),
	_equipped(MakeShared<Equipped>())
{
}

auto Player::GetCharacterId() const -> int64_t
{
	return _character_id;
}

auto Player::SetGender(const int8_t gender) -> void
{
	_gender = gender;
}

auto Player::GetGender() const -> int8_t
{
	return _gender;
}

auto Player::SetName(const std::wstring& name) -> void
{
	_name = name;
}

auto Player::GetName() const -> const wchar_t*
{
	return _name.c_str();
}

auto Player::SetFaceId(const int32_t faceId) -> void
{
	_face_id = faceId;
}

auto Player::GetFaceId() const -> int32_t
{
	return _face_id;
}

auto Player::GetEquipped() const -> std::shared_ptr<Equipped>
{
	return _equipped;
}

