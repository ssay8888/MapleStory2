#include "common_pch.h"
#include "randomizer.h"

#include <random>
#include <boost/random.hpp>
#include <Windows.h>
#include <boost/mpl/bind.hpp>


using namespace boost::random;


auto Randomizer::Rand(const int64_t max)->int64_t
{
	static mt19937 engine(static_cast<unsigned>(time(nullptr)));
	static uniform_int_distribution<int64_t> distribution(0, max); 
	static auto generator = []
	{
		return distribution(engine);
	};
	return generator();
}

auto Randomizer::Rand(const int64_t min, const int64_t max)->int64_t
{
	static mt19937 engine(static_cast<unsigned>(time(nullptr)));
	static uniform_int_distribution<int64_t> distribution(min, max);
	static auto generator = []
	{
		return distribution(engine);
	};
	return generator();
}

auto Randomizer::Rand(const double max) -> double
{
	std::random_device seeder;
	mt19937 rng(seeder());
	const uniform_real_distribution<> gen(0.0, max);
	return gen(rng);
}

auto Randomizer::Rand(const double min, const double max) -> double
{
	std::random_device seeder;
	mt19937 rng(seeder());
	const uniform_real_distribution<> gen(min, max);
	return gen(rng);
}

auto Randomizer::IsSuccess(int32_t rate) -> bool
{
	static mt19937 engine((unsigned int)time(NULL));                    // MT19937 난수 엔진
	static uniform_int_distribution<int64_t> distribution(0, 99);          // 생성 범위
	static auto generator = []
	{
		return distribution(engine);
	};

	if (rate >= generator())
	{
		return true;
	}
	return false;
}
