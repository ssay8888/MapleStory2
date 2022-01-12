#pragma once
#include <string>
#include <vector>

class StringUtils
{
public:
	static auto ConvertWtoC(const wchar_t* str)->std::string;
	static auto ConvertCtoW(const char* str)->std::wstring;

	static auto ToLower(std::wstring& str)->void;
	static auto ToUpper(std::wstring& str)->void;
	static auto ToLower(std::string& str)->void;
	static auto ToUpper(std::string& str)->void;

	static auto Split(const std::string& str, const char delimiter)->std::vector<std::string>;
	static auto Split(const std::wstring& str, const wchar_t delimiter)->std::vector<std::wstring>;
};

