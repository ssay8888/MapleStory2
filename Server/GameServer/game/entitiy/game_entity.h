#pragma once
class Component;
class Transform;

class GameEntity abstract : public std::enable_shared_from_this<GameEntity>
{
public:
	GameEntity();
	~GameEntity() = default;
	
private:
	std::shared_ptr<Transform>	_transform_com = nullptr;
};

