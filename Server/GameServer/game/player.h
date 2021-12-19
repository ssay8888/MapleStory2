#pragma once
class Equipped;

class Player : public std::enable_shared_from_this<Player>
{
public:
	explicit Player(int32_t characterId);


public:
	auto GetCharacterId()const->int32_t;

	auto SetGender(int8_t gender)->void;
	auto GetGender()const->int8_t;

	auto SetName(std::wstring name)->void;
	auto GetName() const ->const wchar_t*;

	auto SetFaceId(int32_t faceId)->void;
	auto GetFaceId()const->int32_t;

	auto GetEquipped() const ->std::shared_ptr<Equipped>;

private:
	int8_t						_gender;
	int32_t						_character_id;
	int32_t						_face_id;
	std::wstring				_name;
	std::shared_ptr<Equipped>	_equipped;
};

