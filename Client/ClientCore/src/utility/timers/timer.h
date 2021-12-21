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
	auto IsTimeCheck(float msTime)->bool;
	auto ResetTotalTime()->void;

public:
	auto GetDeltaTime() const -> float;

private:
	float			_delta_time;
	float			_total_delta_time;
	LARGE_INTEGER	_cpu_tick;
	LARGE_INTEGER	_begin;
	LARGE_INTEGER	_end;
	LARGE_INTEGER	_invalidate;
};

