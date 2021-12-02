#pragma once
#include <vector>
#include "types.h"

class FileUtils
{
public:
	static auto	ReadFile(const WCHAR* path) -> Vector<BYTE>;
	static auto Convert(std::string str) -> String;
	static auto ConvertWtoC(const wchar_t* str)->std::string;
	static auto ConvertCtoW(const char* str)->std::wstring;
};