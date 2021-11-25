#pragma once
class MainApp
{
public:
	MainApp();
	~MainApp() = default;

public:
	auto NativeConstruct()->HRESULT;
	auto Tick(double timeDelta)->int32_t;
	auto RenderMainApp()->HRESULT;

private:
	HRESULT AddPrototypeGameObject();
	HRESULT AddPrototypeComponent();

private:
	ComPtr<IDirect3DDevice9> _graphic_device;
};

