#include "common_pch.h"
#include "string_utils.h"

#include <algorithm>
#include <sstream>
#include <winstring.h>

auto StringUtils::ConvertWtoC(const wchar_t* str) -> std::string
{
	char pStr[MAX_PATH];
	const int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);

	return pStr;
}

auto StringUtils::ConvertCtoW(const char* str) -> std::wstring
{
	wchar_t pStr[MAX_PATH];
	const int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, str, static_cast<int32_t>(strlen(str)) + 1, pStr, strSize);
	return pStr;
}

auto StringUtils::ToLower(std::wstring& str) -> void
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

auto StringUtils::ToUpper(std::wstring& str) -> void
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

auto StringUtils::Split(const std::string& str, const char delimiter) -> std::vector<std::string>
{
	std::vector<std::string> answer;
	std::stringstream ss(str);
	std::string temp;

	while (getline(ss, temp, delimiter)) {
		answer.push_back(temp);
	}

	return answer;
}

auto StringUtils::Split(const std::wstring& str, const wchar_t delimiter) -> std::vector<std::wstring>
{
	std::vector<std::wstring> answer;
	std::wstringstream ss(str);
	std::wstring temp;

	while (getline(ss, temp, delimiter)) {
		answer.push_back(temp);
	}

	return answer;
}
