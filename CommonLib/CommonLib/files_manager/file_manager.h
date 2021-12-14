#pragma once
#include <string>
#include <list>
class FileManager
{
private:
    FileManager() = default;
    ~FileManager() = default;

public:
    static auto ReadFile(std::wstring path)->std::string;
    static auto GetDirFileCount(std::wstring folderPath)->int;
    static auto GetDirFileName(std::wstring folderPath)->std::list<std::wstring>;

    static auto GetFileName(std::wstring path)->std::wstring;
    static auto GetFileName(std::string path)->std::string;
    static auto GetPath(std::wstring path)->std::wstring;
    static auto GetPath(std::string path)->std::string;

    static auto IsFileAccess(std::string path)->bool;
};
