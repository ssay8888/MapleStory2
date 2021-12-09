#include "common_pch.h"
#include "file_manager.h"

#include <fstream>
#include <Windows.h>
#include <iostream>
#include <string>

auto FileManager::ReadFile(std::wstring path) const -> std::string
{
	std::ifstream fin(path);
	std::string temp, str;
	if (fin.is_open())
	{
		while (std::getline(fin, temp))
		{
			str.append(temp).append("\n");
		}
	}
	fin.close();
	return str;
}

auto FileManager::GetDirFileCount(std::wstring folderPath) -> int
{
	WIN32_FIND_DATAW data;
	int size = 0;
	const HANDLE hFind = FindFirstFileW(folderPath.append(L"*.txt").c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			size++;
		} while (FindNextFileW(hFind, &data));
		FindClose(hFind);
	}
	return size;
}

auto FileManager::GetDirFileName(std::wstring folderPath) -> std::list<std::wstring>
{
	std::list<std::wstring> list;
	WIN32_FIND_DATAW data;
	int size = 0;
	std::wstring folderPath_(folderPath);
	const HANDLE hFind = FindFirstFileW(folderPath_.append(L"*.*").c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wcscmp(data.cFileName, L".") && wcscmp(data.cFileName, L".."))
			{
				std::wstring path(folderPath);
				path.append(data.cFileName);
				list.emplace_back(path);
			}
		} while (FindNextFileW(hFind, &data));
		FindClose(hFind);
	}
	return list;
}

auto FileManager::GetFileName(std::wstring path) -> std::wstring
{
	auto strs = wcsrchr(path.c_str(), '\\');
	if (strs == NULL)
	{
		return path;
	}
	else
	{
		return strs + 1;
	}
}


auto FileManager::GetFileName(std::string path) -> std::string
{
	auto strs = strrchr(path.c_str(), '\\');
	if (strs == NULL)
	{
		return path;
	}
	else
	{
		return strs + 1;
	}
}
