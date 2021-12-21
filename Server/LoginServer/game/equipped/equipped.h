#pragma once

class Equipped
{
public:
	Equipped() = default;

public:
	auto PushItem(int32_t itemId)->void;
	auto FindItem(GameContents::kEquipeType type)->int32_t;
	auto GetAllItem()const->std::vector<int32_t>;

private:
	std::map<GameContents::kEquipeType, int32_t> _eqp_list;
};

