#include "pch.h"
#include "timer.h"

auto Timer::GetDeltaTime() const -> float
{
	return _delta_time;
}


auto Timer::ReadyTimeManager() -> HRESULT
{
	QueryPerformanceFrequency(&_cpu_tick);
	QueryPerformanceCounter(&_begin);
	QueryPerformanceCounter(&_end);
	QueryPerformanceCounter(&_invalidate);
	return S_OK;
}

auto Timer::UpdateTimeManager() -> void
{
	QueryPerformanceCounter(&_end);

	if ((_end.QuadPart - _invalidate.QuadPart) > _cpu_tick.QuadPart)
	{
		_invalidate = _end;
		QueryPerformanceFrequency(&_cpu_tick);
	}


	_delta_time = static_cast<float>(_end.QuadPart - _begin.QuadPart) / _cpu_tick.QuadPart;

	_begin = _end;
}
