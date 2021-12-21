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

private:


private:
	tbb::concurrent_hash_map<int64_t, std::shared_ptr<AuthInfo>> _auth_storage;
};

