#pragma once
#include <wrl.h>
#include <d3d9.h>
#include <d3dx9.h>

using namespace Microsoft::WRL;

class GraphicDevice
{
private:
	GraphicDevice();
	~GraphicDevice();

public:
	enum class WINMODE { MODE_FULL, MODE_WIN };

public:
	static auto& GetInstance()
	{
		static GraphicDevice instance;
		return instance;
	}

public:
	auto GetDevice() const->ComPtr<IDirect3DDevice9>;
	auto GetFont() const->ComPtr<ID3DXFont>;
	auto GetLine() const->ComPtr<ID3DXLine>;

public:
	auto ReadyGraphicDevice(HWND hWnd, GraphicDevice::WINMODE mode, uint32_t winCx, uint32_t winCy, ComPtr<IDirect3DDevice9>* ppGraphicDevice = nullptr)->HRESULT;
	auto RenderBegin()->void;
	auto RenderEnd(HWND hWND = nullptr)->void;

private:
	ComPtr<IDirect3D9>				_sdk;
	ComPtr<IDirect3DDevice9>		_device;
	ComPtr<ID3DXFont>				_font;
	ComPtr<ID3DXLine>				_line;
};

