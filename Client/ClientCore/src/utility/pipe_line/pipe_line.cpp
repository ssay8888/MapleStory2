#include "pch.h"
#include "pipe_line.h"

PipeLine::PipeLine()
{
	D3DXMatrixIdentity(&_view_matrix);
	D3DXMatrixIdentity(&_proj_matrix);
}

_matrix PipeLine::GetTransform(const D3DTRANSFORMSTATETYPE stateType)
{
	return stateType == D3DTS_VIEW ? _view_matrix : _proj_matrix;
}

auto PipeLine::GetCamPosition() const -> _float3
{
	return _cam_position;
}

auto PipeLine::SetTransform(D3DTRANSFORMSTATETYPE eStateType, const _matrix& transformMatrix) -> void
{
	if (eStateType == D3DTS_VIEW)
	{
		_view_matrix = transformMatrix;
		/*캠의 월드매트릭스를 구해온다.*/
		_matrix		viewMatrixInverse;
		D3DXMatrixInverse(&viewMatrixInverse, nullptr, &_view_matrix);
		memcpy(&_cam_position, &viewMatrixInverse.m[3][0], sizeof(_float3));
	}
	else
	{
		_proj_matrix = transformMatrix;
	}
}

