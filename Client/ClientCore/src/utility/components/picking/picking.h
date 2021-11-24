#pragma once
class ViBuffer;

class Picking
{
public:
	static auto GetInstance()->Picking&
	{
		static Picking instance;
		return instance;
	}

public:
	auto ComputeMouseCursorPosInWorld(HWND hWnd)->HRESULT;
	auto ComputePickingPoint(const std::shared_ptr<ViBuffer>& viBuffer, const _matrix worldMatrix)->const _float3*;
private:
	_float3	_mouse_ray;
	_float3	_mouse_pivot;
	_float3	_result_pos;

};

