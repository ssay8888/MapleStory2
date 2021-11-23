#include "pch.h"
#include "timer_manager.h"
#include "timer.h"
#include "src/defines/engine_functor.h"

TimerManager::TimerManager()
{
}

TimerManager::~TimerManager()
{
}

auto TimerManager::AddTimers(const wchar_t* TimerTag) -> HRESULT
{
	if (nullptr != FindTimers(TimerTag))
		return E_FAIL;

	auto pTimer = MakeShared<Timer>();
	if (nullptr == pTimer)
		return E_FAIL;

	_timers.insert(TIMERS::value_type(TimerTag, pTimer));

	return S_OK;
}

auto TimerManager::ComputeTimeDelta(const wchar_t* TimerTag) -> float_t
{
	auto Timer = FindTimers(TimerTag);
	if (nullptr == Timer)
		return 0.0f;

	Timer->UpdateTimeManager();

	return Timer->GetDeltaTime();
}

auto TimerManager::FindTimers(const wchar_t* pTimerTag) -> std::shared_ptr<Timer>
{
	auto	iter = find_if(_timers.begin(), _timers.end(), TagFinder(pTimerTag));
	if (iter == _timers.end())
		return nullptr;

	return iter->second;
}
