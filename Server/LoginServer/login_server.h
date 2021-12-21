#pragma once
class LoginServer
{
public:
	enum
	{
		kWorkerTick = 64
	};

	LoginServer() = default;

	auto LoginServerInit()->void;

	auto LoginServerServiceCreate()->void;
	auto LoginCenterServerServiceCreate()->void;

	auto GetCenterServerSevice() const ->ServerServiceRef;
	auto GetLoginServerSevice() const ->ServerServiceRef;
private:
	std::atomic<bool> _exit_loop = false;
	ServerServiceRef _login_service;
	ServerServiceRef _center_service;
};

