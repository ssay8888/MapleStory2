#include "pch.h"
#include "dead_lock_profiler.h"

auto DeadLockProfiler::PushLock(const char* name) -> void
{
	LockGuard guard(_lock);
	
	int32 lockId = 0;

	const auto findIt = _name_to_id.find(name);
	if (findIt == _name_to_id.end())
	{
		lockId = static_cast<int32>(_name_to_id.size());
		_name_to_id[name] = lockId;
		_id_to_name[lockId] = name;
	}
	else
	{
		lockId = findIt->second;
	}
	
	if (LLockStack.empty() == false)
	{
		const int32 prevId = LLockStack.top();
		if (lockId != prevId)
		{
			std::set<int32>& history = _lock_history[prevId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	LLockStack.push(lockId);
}

auto DeadLockProfiler::PopLock(const char* name) -> void
{
	LockGuard guard(_lock);

	if (LLockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	const int32 lockId = _name_to_id[name];
	if (LLockStack.top() != lockId)
		CRASH("INVALID_UNLOCK");

	LLockStack.pop();
}

auto DeadLockProfiler::CheckCycle() -> void
{
	const auto lockCount = static_cast<int32>(_name_to_id.size());
	_discovered_order = std::vector<int32>(lockCount, -1);
	_discovered_count = 0;
	_finished = std::vector<bool>(lockCount, false);
	_parent = std::vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
		Dfs(lockId);
	
	_discovered_order.clear();
	_finished.clear();
	_parent.clear();
}

auto DeadLockProfiler::Dfs(const int32 here) -> void
{
	if (_discovered_order[here] != -1)
		return;

	_discovered_order[here] = _discovered_count++;

	const auto findIt = _lock_history.find(here);
	if (findIt == _lock_history.end())
	{
		_finished[here] = true;
		return;
	}

	const std::set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		if (_discovered_order[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}
		
		if (_discovered_order[here] < _discovered_order[there])
			continue;
		
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _id_to_name[here], _id_to_name[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _id_to_name[_parent[now]], _id_to_name[now]);
				now = _parent[now];
				if (now == there)
					break;
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}

	_finished[here] = true;
}
