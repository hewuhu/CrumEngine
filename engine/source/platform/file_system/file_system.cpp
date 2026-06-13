module crumb.platform.file_system;

import std;

namespace Crumb {

// ============================================================================
// FileSystem::getFiles — 递归遍历，返回所有普通文件
// ============================================================================
std::vector<std::filesystem::path> FileSystem::getFiles(const std::filesystem::path& directory)
{
    std::vector<std::filesystem::path> files;
    for (auto const& entry : std::filesystem::recursive_directory_iterator{directory})
    {
        if (entry.is_regular_file())
        {
            files.push_back(entry);
        }
    }
    return files;
}

// ============================================================================
// Path::getRelativePath — 将 file_path 转为相对于 directory 的路径
// ============================================================================
const std::filesystem::path Path::getRelativePath(
    const std::filesystem::path& directory,
    const std::filesystem::path& file_path)
{
    return file_path.lexically_relative(directory);
}

// ============================================================================
// Path::getPathSegments — 将路径拆分为各段字符串
// ============================================================================
const std::vector<std::string> Path::getPathSegments(const std::filesystem::path& file_path)
{
    std::vector<std::string> segments;
    for (auto iter = file_path.begin(); iter != file_path.end(); ++iter)
    {
        segments.emplace_back(iter->generic_string());
    }
    return segments;
}

// ============================================================================
// Path::getFileExtensions — 提取三层扩展名
// 例如 "models/hero.component.json"
//   → (".json",    ".component",   ".")
//       extension   stem.extension  stem.stem.extension
// ============================================================================
const std::tuple<std::string, std::string, std::string>
Path::getFileExtensions(const std::filesystem::path& file_path)
{
    return std::make_tuple(
        file_path.extension().generic_string(),
        file_path.stem().extension().generic_string(),
        file_path.stem().stem().extension().generic_string());
}

// ============================================================================
// Path::getFilePureName — 去掉扩展名
// 例如 "foo.component.json" → "foo"
// ============================================================================
const std::string Path::getFilePureName(const std::string file_full_name)
{
    std::string result = file_full_name;
    auto pos = file_full_name.find_first_of('.');
    if (pos != std::string::npos)
    {
        result = file_full_name.substr(0, pos);
    }
    return result;
}

} // namespace Crumb
