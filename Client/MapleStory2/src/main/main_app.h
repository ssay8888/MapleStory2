#pragma once
#include "src/network/service.h"

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
	auto ClientNetworkThreadInit(const std::wstring& ip, const int16_t port, Service::kServerType type, int32_t sessionCount, int32_t threadCount)->ClientServiceRef;
	auto WaitConnectServer()->void;
	auto NetworkThreadStopForWait()->void;

public:
	auto IsConnected() const ->bool;
	auto SetConnected(bool connected)->void;

	auto SetAuthInfo(Protocol::LoginServerCharacterSelect info)->void;
	auto GetAuthInfo()const->Protocol::LoginServerCharacterSelect;

private:
	auto AddPrototypeGameObject()->HRESULT;
	auto AddPrototypeComponent()->HRESULT;

private:
	ComPtr<IDirect3DDevice9>		 _graphic_device;
	std::atomic<bool>				 _is_connected;
	std::atomic<bool>				_exit;
	std::atomic<int>				_iocp_thread_running_count;

	uint64_t				m_dwNumRender = 0;
	WCHAR					m_szFPS[MAX_PATH] = TEXT("");
	double					m_TimeAcc = 0.0;

	Protocol::LoginServerCharacterSelect _auth_info;
};

