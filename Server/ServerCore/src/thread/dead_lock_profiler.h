#pragma once
#include <stack>
#include <map>
#include <vector>

class DeadLockProfiler
{
private:
	DeadLockProfiler() = default;
	~DeadLockProfiler() = default;

public:
	static auto GetInstance() -> DeadLockProfiler&
	{
		static DeadLockProfiler instance;
		return instance;
	}

public:
	auto PushLock(const char* name) -> void;
	auto PopLock(const char* name) -> void;
	auto CheckCycle() -> void;

private:
	auto Dfs(int32_t index) -> void;

private:
	std::unordered_map<const char*, int32_t>	_name_to_id;
	std::unordered_map<int32_t, const char*>	_id_to_name;
	std::map<int32_t, std::set<int32_t>>		_lock_history;

	Mutex _lock;

private:
	std::vector<int32_t>	_discovered_order;
	int32_t				_discovered_count = 0;
	std::vector<bool>	_finished;
	std::vector<int32_t>	_parent;
};

