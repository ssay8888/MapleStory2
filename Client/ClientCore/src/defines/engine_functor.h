#pragma once
#include <string>

class TagFinder
{
public:
	TagFinder(const std::wstring& pTargetTag) : _target_tag(pTargetTag) {}
	~TagFinder() = default;

public:
	template <typename T>
	bool operator () (T pair)
	{
		if (pair.first == _target_tag)
			return true;

		return false;
	}

private:
	const std::wstring _target_tag = nullptr;
};
