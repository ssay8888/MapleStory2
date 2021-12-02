#pragma once
class PipeLine
{
private:
	PipeLine();
	~PipeLine() = default;

public:
	static auto GetInstance()->PipeLine&
	{
		static PipeLine instance;
		return instance;
	}

public:
	auto GetTransform(const D3DTRANSFORMSTATETYPE stateType)->_matrix;
	auto GetCamPosition() const->_float3;

public:
	auto SetTransform(D3DTRANSFORMSTATETYPE eStateType, const _matrix& transformMatrix)->void;

private:
	_matrix			_view_matrix;
	_matrix			_proj_matrix;
	_float3			_cam_position;
};

