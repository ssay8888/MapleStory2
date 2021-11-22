#pragma once
#include <d3dx9math.h>

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	enum kMatrixState
	{
		kStateRight,
		kStateUp,
		kStateLook,
		kStatePosition,
	};
	enum kObjectState
	{
		kWait,
		kAlive,
		kDead
	};
	GameObject();
	~GameObject() = default;

public:
	auto SetMatrixState(kMatrixState state, D3DXVECTOR3 value) -> void;
	auto SetState(kObjectState objectState) -> void;
	auto GetState() const-> kObjectState;

	auto SetWorldPos(D3DXMATRIX world) -> void;
	auto GetWorldPos() const -> D3DXMATRIX;
private:
	D3DXMATRIX _world_pos;
	kObjectState _state;
};

