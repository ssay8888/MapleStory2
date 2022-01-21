#pragma once
class DamageManager
{
private:
	DamageManager() = default;
public:
	static auto GetInstance()->DamageManager&
	{
		static DamageManager instance;
		return instance;
	}

private:

};

