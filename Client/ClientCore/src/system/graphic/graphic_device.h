#pragma once
#include <d3d9.h>
#include <d3dx9.h>


class GraphicDevice
{
private:
	GraphicDevice() = default;
	~GraphicDevice() = default;

public:
	enum class kWindowMode
	{
		kModeFull,
		kModeWin
	};

public:
	static auto GetInstance() -> GraphicDevice&
	{
		static GraphicDevice instance;
		return instance;
	}

public:
	auto GetDevice() const->ComPtr<IDirect3DDevice9>;
	auto GetFont() const->ComPtr<ID3DXFont>;
	auto GetLine() const->ComPtr<ID3DXLine>;

public:
	auto ReadyGraphicDevice(HWND hWnd, kWindowMode mode, uint32_t winCx, uint32_t winCy, ComPtr<IDirect3DDevice9>* ppGraphicDevice = nullptr)->HRESULT;
	auto RenderBegin()->void;
	auto RenderEnd(HWND hWnd = nullptr)->void;

private:
	ComPtr<IDirect3D9>				_sdk;
	ComPtr<IDirect3DDevice9>		_device;
	ComPtr<ID3DXFont>				_font;
	ComPtr<ID3DXLine>				_line;
};

