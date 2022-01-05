#pragma once
#include <cstdint>
#include <boost/random/mersenne_twister.hpp>

class Randomizer
{
public:
	static auto GetInstance() ->Randomizer&
	{
		static Randomizer instance;
		return instance;
	}

	static auto Rand(int64_t max)->int64_t;
	static auto Rand(int64_t min, int64_t max)->int64_t;

	static auto Rand(double max)->double;
	static auto Rand(double min, double max)->double;

	static auto IsSuccess(int32_t rate)->bool;
};

