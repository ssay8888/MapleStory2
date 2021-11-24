#include "pch.h"
#include "picking.h"

#include "src/utility/vi_buffer/vi_buffer.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/pipe_line/pipe_line.h"

auto Picking::ComputeMouseCursorPosInWorld(const HWND hWnd) -> HRESULT
{
	_float4		mousePos;
	POINT		mouse = { 0, 0 };
	GetCursorPos(&mouse);
	ScreenToClient(hWnd, &mouse);

	D3DVIEWPORT9			viewPort;
	GraphicDevice::GetInstance().GetDevice()->GetViewport(&viewPort);
	mousePos.x = static_cast<float>(mouse.x) / (static_cast<float>(viewPort.Width) * 0.5f) - 1.f;
	mousePos.y = static_cast<float>(mouse.y) / -(static_cast<float>(viewPort.Height) * 0.5f) + 1.f;
	mousePos.z = 0.f;
	mousePos.w = 1.f;

	auto& pPipeLine = PipeLine::GetInstance();

	_matrix			projMatrix;
	projMatrix = pPipeLine.GetTransform(D3DTS_PROJECTION);
	D3DXVec4Transform(&mousePos, &mousePos, D3DXMatrixInverse(&projMatrix, nullptr, &projMatrix));

	_mouse_ray = _float3(mousePos.x , mousePos.y, mousePos.z);
	_mouse_pivot = _float3(0.f, 0.f, 0.f);

	_matrix			viewMatrix;
	viewMatrix = pPipeLine.GetTransform(D3DTS_VIEW);
	D3DXMatrixInverse(&viewMatrix, nullptr, &viewMatrix);

	D3DXVec3TransformCoord(&_mouse_pivot, &_mouse_pivot, &viewMatrix);
	D3DXVec3TransformNormal(&_mouse_ray, &_mouse_ray, &viewMatrix);
	return S_OK;
}

auto Picking::ComputePickingPoint(const std::shared_ptr<ViBuffer>& viBuffer, const _matrix worldMatrix) -> const _float3*
{
	_float3		ray, pivot;
	_matrix		worldMatrixInverse = *D3DXMatrixInverse(&worldMatrixInverse, nullptr, &worldMatrix);

	D3DXVec3TransformCoord(&pivot, &_mouse_pivot, &worldMatrixInverse);
	D3DXVec3TransformNormal(&ray, &_mouse_ray, &worldMatrixInverse);

	for (uint32_t i = 0; i < viBuffer->GetNumPolygons(); ++i)
	{
		_float3 points[3];
		viBuffer->GetPolygonVertexPositions(i, points);

		float		u, v, dist;
		if (TRUE == D3DXIntersectTri(&points[0], &points[1], &points[2], &pivot, &ray, &u, &v, &dist))
		{
			_result_pos = pivot + *D3DXVec3Normalize(&ray, &ray) * dist;
			return &_result_pos;
		}
	}
	return nullptr;
}
