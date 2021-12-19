#include "common_pch.h"
#include "game_contents.h"

auto GameContents::EquipeType(const int32_t itemId) -> kEquipeType
{
	if (itemId >= 300000 && itemId <= 300999)
	{
		return kEquipeType::kFace;
	}
	else if (itemId >= 11400000 && itemId <= 11400999)
	{
		return kEquipeType::kCoat;
	}
	else if (itemId >= 11500000 && itemId <= 11500999)
	{
		return kEquipeType::kPants;
	}
	else if (itemId >= 11700000 && itemId <= 11700999)
	{
		return kEquipeType::kShoes;
	}

	return kEquipeType::kNull;
}
