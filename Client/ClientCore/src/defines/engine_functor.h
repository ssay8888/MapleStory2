#pragma once

class TagFinder
{
public:
	TagFinder(const wchar_t* pTargetTag) : _target_tag(pTargetTag) {}
	~TagFinder() = default;

public:
	template <typename T>
	bool operator () (T pair)
	{
		if (0 == lstrcmp(pair.first, _target_tag))
			return true;

		return false;
	}

private:
	const wchar_t* _target_tag = nullptr;
};