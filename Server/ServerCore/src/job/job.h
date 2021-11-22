#pragma once
#include <functional>

using CallbackType = std::function<void()>;

class Job
{
public:
	explicit Job(CallbackType&& callback);

	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args);

	void Execute() const;

private:
	CallbackType _callback;
};

template <typename T, typename Ret, typename ... Args>
Job::Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
{
	_callback = [owner, memFunc, args...]()
	{
		(owner.get()->*memFunc)(args...);
	};
}

