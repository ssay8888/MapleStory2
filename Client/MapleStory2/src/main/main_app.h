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

public:
	auto NetworkThreadInit()->ClientServiceRef;
	auto WaitConnectServer()->void;

public:
	auto IsConnected() const ->bool;
	auto SetConnected(bool connected)->void;

private:
	auto AddPrototypeGameObject()->HRESULT;
	auto AddPrototypeComponent()->HRESULT;

private:
	ComPtr<IDirect3DDevice9>		 _graphic_device;
	std::atomic<bool>				 _is_connected;
	static std::atomic<bool>		 _exit;
};

