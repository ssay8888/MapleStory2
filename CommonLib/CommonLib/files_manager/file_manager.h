#pragma once
#include <string>
#include <list>
class FileManager
{
private:
    FileManager() = default;
    ~FileManager() = default;

public:
    static auto GetInstance()->FileManager& {
        static FileManager instance;
        return instance;
    }

    auto ReadFile(std::wstring path) const->std::string;
    auto GetDirFileCount(std::wstring folderPath)->int;
    auto GetDirFileName(std::wstring folderPath)->std::list<std::wstring>;

    auto GetFileName(std::wstring path)->std::wstring;
    auto GetFileName(std::string path)->std::string;
    auto GetPath(std::wstring path)->std::wstring;
    auto GetPath(std::string path)->std::string;

    auto IsFileAccess(std::string path)->bool;
};
