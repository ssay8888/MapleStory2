#pragma once
#include <string>

class StringUtils
{
public:
	static auto ConvertWtoC(const wchar_t* str)->std::string;
	static auto ConvertCtoW(const char* str)->std::wstring;
};

