#pragma once
class BaseInfo abstract : std::enable_shared_from_this<BaseInfo>
{
public:
	explicit BaseInfo(int64_t characterId);
	virtual ~BaseInfo() = default;

public:
	auto GetCharacterId()const->int64_t;

protected:
	int64_t _character_id;
};

