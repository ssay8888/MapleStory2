#include "pch.h"
#include "file_utils.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

Vector<BYTE> FileUtils::ReadFile(const WCHAR* path)
{
	Vector<BYTE> ret;

	const fs::path filePath{ path };

	const auto fileSize = static_cast<uint32_t>(fs::file_size(filePath));
	ret.resize(fileSize);

	std::basic_ifstream<BYTE> inputStream{ filePath };
	inputStream.read(&ret[0], fileSize);

	return ret;
}

String FileUtils::Convert(std::string str)
{
	const auto srcLen = static_cast<int32_t>(str.size());

	String ret;
	if (srcLen == 0)
		return ret;

	const int32_t retLen = ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, NULL, 0);
	ret.resize(retLen);
	::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, &ret[0], retLen);

	return ret;
}

auto FileUtils::ConvertWtoC(const wchar_t* str)->std::string
{
	char pStr[MAX_PATH];
	const int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);

	return pStr;
}

auto FileUtils::ConvertCtoW(const char* str) -> std::wstring
{
	wchar_t pStr[MAX_PATH];
	const int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
	return pStr;
}
