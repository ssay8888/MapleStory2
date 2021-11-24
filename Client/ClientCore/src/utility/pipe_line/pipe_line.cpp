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

auto PipeLine::SetTransform(D3DTRANSFORMSTATETYPE eStateType, const _matrix& transformMatrix) -> void
{
	if (eStateType == D3DTS_VIEW)
	{
		_view_matrix = transformMatrix;
	}
	else
	{
		_proj_matrix = transformMatrix;
	}
}

