#pragma once
#include <cstdint>

class GameContents
{
public:
#pragma region Equip
	enum class kEquipeType
	{
		kNull,
		kCoat = -1,
		kPants = -2,
		kFace = -3,
		kShoes = -4,
		kWeapon = -5
	};
	static auto EquipeType(int32_t itemId)->kEquipeType;
#pragma endregion

};

