#pragma once
class Timer;
class TimerManager
{
private:
	TimerManager();
	~TimerManager();

public:
	static auto GetInstance() -> TimerManager&
	{
		static TimerManager instance;
		return instance;
	}

public:
	auto AddTimers(const wchar_t* TimerTag)->HRESULT;
	auto ComputeTimeDelta(const wchar_t* TimerTag)->float_t;


private:
	auto FindTimers(const wchar_t* pTimerTag)->std::shared_ptr<Timer>;

private:
	std::map<const wchar_t*, std::shared_ptr<Timer>>			_timers;
	typedef std::map<const wchar_t*, std::shared_ptr<Timer>>	TIMERS;
};

