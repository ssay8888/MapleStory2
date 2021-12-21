#pragma once

class Timer;

class TimerManager
{
private:
	TimerManager() = default;
	~TimerManager() = default;

public:
	static auto GetInstance() -> TimerManager&
	{
		static TimerManager instance;
		return instance;
	}

public:
	auto AddTimers(const wchar_t* timerTag)->HRESULT;
	auto ComputeTimeDelta(const wchar_t* timerTag)->float_t;
	auto GetTimeDelta(const wchar_t* timerTag)->float_t;

	auto IsTimeCheck(const wchar_t* timerTag, float msTime)->bool;
	auto ResetTime(const wchar_t* timerTag)->void;


private:
	auto FindTimers(const wchar_t* timerTag)->std::shared_ptr<Timer>;

private:
	std::map<const wchar_t*, std::shared_ptr<Timer>>			_timers;
	typedef std::map<const wchar_t*, std::shared_ptr<Timer>>	Timers;
}; 

