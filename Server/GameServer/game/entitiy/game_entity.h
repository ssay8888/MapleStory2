#pragma once
class Component;

class GameEntity abstract : public std::enable_shared_from_this<GameEntity>
{
public:
	GameEntity();
	virtual ~GameEntity() = default;
	
private:
};

