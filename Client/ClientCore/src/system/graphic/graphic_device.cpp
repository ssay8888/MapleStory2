#include "pch.h"
#include "graphic_device.h"

auto GraphicDevice::GetDevice() const ->ComPtr<IDirect3DDevice9>
{
	return _device;
}

auto GraphicDevice::GetFont() const -> ComPtr<ID3DXFont>
{
	return _font;
}

auto GraphicDevice::GetLine() const -> ComPtr<ID3DXLine>
{
	return _line;
}

auto GraphicDevice::ReadyGraphicDevice(HWND hWnd, GraphicDevice::kWindowMode mode, uint32_t winCx, uint32_t winCy,
	ComPtr<IDirect3DDevice9>* ppGraphicDevice) -> HRESULT
{
	D3DCAPS9 d3dcaps;
	ZeroMemory(&d3dcaps, sizeof(D3DCAPS9));
	_sdk.Attach(Direct3DCreate9(D3D_SDK_VERSION));
	if (FAILED(_sdk->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps)))
	{
		MSGBOX("Creating Failed Graphic_Device ");
	}
	DWORD vp = 0;
	if (d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.BackBufferWidth = winCx;
	d3dpp.BackBufferHeight = winCy;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;

	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = static_cast<bool>(mode);
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(_sdk->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &d3dpp, &_device)))
	{
		MSGBOX("Creating Failed Graphic_Device ");
	}

	D3DXFONT_DESCW tFontDesc;
	ZeroMemory(&tFontDesc, sizeof(D3DXFONT_DESCW));
	tFontDesc.Height = 15;
	tFontDesc.Width = 16;
	tFontDesc.Weight = FW_HEAVY;
	tFontDesc.CharSet = HANGUL_CHARSET;
	lstrcpy(tFontDesc.FaceName, L"@³Ø½¼Lv1°íµñ Bold");

	if (FAILED(D3DXCreateFontIndirectW(_device.Get(), &tFontDesc, &_font)))
	{
		MSGBOX("Creating Failed Graphic_Device ");
	}
	if (FAILED(D3DXCreateLine(_device.Get(), &_line)))
	{
		MSGBOX("Creating Failed Graphic_Device ");
	}

	if (nullptr != ppGraphicDevice)
	{
		*ppGraphicDevice = _device;
	}

	return S_OK;
}

auto GraphicDevice::RenderBegin() -> void
{
	_device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0, 0, 255, 255), 1.f, 0);
	if (FAILED(_device->BeginScene()))
	{
		return;
	}
}

auto GraphicDevice::RenderEnd(HWND hWnd) -> void
{
	if (FAILED(_device->EndScene()))
	{
		return;
	}
	
	_device->Present(nullptr, nullptr, hWnd, nullptr);
}
