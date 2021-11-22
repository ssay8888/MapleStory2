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
	auto Dfs(int32 index) -> void;

private:
	std::unordered_map<const char*, int32>	_name_to_id;
	std::unordered_map<int32, const char*>	_id_to_name;
	std::map<int32, std::set<int32>>		_lock_history;

	Mutex _lock;

private:
	std::vector<int32>	_discovered_order;
	int32				_discovered_count = 0;
	std::vector<bool>	_finished;
	std::vector<int32>	_parent;
};

