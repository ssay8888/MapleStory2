#include "common_pch.h"
#include "string_utils.h"

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
