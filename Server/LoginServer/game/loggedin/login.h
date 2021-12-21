#pragma once
#include "login_session/login_session_manaeger.h"

class Login
{
public:
	enum class kLoginState
	{
		kNotLoggedin,
		kLoggedin
	};
	explicit Login(LoginSessionRef session);
	~Login() = default;

public:
	auto Connected()->void;
	auto Disconnected()->void;

private:
	std::weak_ptr<LoginSession> _session;
	kLoginState					_state;
};