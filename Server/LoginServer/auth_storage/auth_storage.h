#pragma once
#include "login_session/login_session_manaeger.h"

class AuthStorage
{
public:
	static auto GetInstance() -> AuthStorage&
	{
		static AuthStorage instance;
		return instance;
	}
	struct AuthInfo
	{
		LoginSessionRef session;
		std::int64_t auth;
	};

	auto CreateAuth(LoginSessionRef session)const->std::shared_ptr<AuthInfo>;
	auto AddAuth(std::shared_ptr<AuthInfo> auth)->void;
	auto RemoveAuth(int64_t accountId)->bool;
	auto FindAuth(int64_t accountId) const ->std::shared_ptr<AuthInfo>;
private:


private:
	tbb::concurrent_hash_map<int64_t, std::shared_ptr<AuthInfo>> _auth_storage;
};

