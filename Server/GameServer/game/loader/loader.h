#pragma once
class Loader
{
private:
	Loader();

public:
	static auto GetInstance()->Loader
	{
		static Loader instance;
		return instance;
	}

private:
	auto Init()-> void;

	auto ComponentLoad()->HRESULT;

};

