#pragma once
#include <iostream>

class Timer : public std::enable_shared_from_this<Timer>
{
public:
	Timer() = default;
	~Timer() = default;

public:
	auto ReadyTimeManager()->HRESULT;
	auto UpdateTimeManager()->void;

public:
	auto GetDeltaTime() const -> float;

private:
	float			_delta_time;
	LARGE_INTEGER	_cpu_tick;
	LARGE_INTEGER	_begin;
	LARGE_INTEGER	_end;
	LARGE_INTEGER	_invalidate;
};
