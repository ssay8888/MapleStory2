#include "pch.h"
#include "timer_manager.h"
#include "timer.h"
#include "src/defines/engine_functor.h"

auto TimerManager::AddTimers(const wchar_t* timerTag) -> HRESULT
{
	if (nullptr != FindTimers(timerTag))
	{
		return E_FAIL;
	}

	auto pTimer = std::make_shared<Timer>();
	if (nullptr == pTimer)
	{
		return E_FAIL;
	}

	if (!_timers.insert(Timers::value_type(timerTag, pTimer)).second)
	{
		return E_FAIL;
	}

	return S_OK;
}

auto TimerManager::ComputeTimeDelta(const wchar_t* timerTag) -> float_t
{
	const auto timer = FindTimers(timerTag);
	if (nullptr == timer)
	{
		return 0.0f;
	}

	timer->UpdateTimeManager();

	return timer->GetDeltaTime();
}

auto TimerManager::GetTimeDelta(const wchar_t* timerTag)->float_t
{
	const auto timer = FindTimers(timerTag);
	if (nullptr == timer)
	{
		return 0.0f;
	}

	return timer->GetDeltaTime();
}

auto TimerManager::IsTimeCheck(const wchar_t* timerTag, const float msTime) -> bool
{
	const auto timer = FindTimers(timerTag);
	if (nullptr == timer)
	{
		return false;
	}

	return timer->IsTimeCheck(msTime);
}

auto TimerManager::ResetTime(const wchar_t* timerTag) -> void
{
	const auto timer = FindTimers(timerTag);
	if (nullptr == timer)
	{
		return;

	}
	timer->ResetTotalTime();
}

auto TimerManager::FindTimers(const wchar_t* timerTag) -> std::shared_ptr<Timer>
{
	const auto	iterator = find_if(_timers.begin(), _timers.end(), TagFinder(timerTag));
	if (iterator == _timers.end())
		return nullptr;

	return iterator->second;
}
