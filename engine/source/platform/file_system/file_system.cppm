export module crumb.platform.file_system;

import std;

// ============================================================================
// FileSystem + Path — 平台文件系统抽象层
// 提供目录遍历、路径解析、扩展名提取等基础操作
// （参考 Piccolo 引擎 platform 层，适配 C++ 模块体系）
// ============================================================================
export namespace Crumb {

// ============================================================================
// FileSystem — 文件系统遍历
// ============================================================================
class FileSystem {
public:
    /// @brief 递归遍历目录下所有普通文件
    /// @param directory 要遍历的目录路径
    /// @return 所有普通文件的路径列表
    std::vector<std::filesystem::path> getFiles(const std::filesystem::path& directory);
};

// ============================================================================
// Path — 路径工具（全静态方法）
// ============================================================================
class Path {
public:
    /// @brief 计算 file_path 相对于 directory 的路径
    static const std::filesystem::path getRelativePath(
        const std::filesystem::path& directory,
        const std::filesystem::path& file_path);

    /// @brief 将路径拆分为各段（如 "a/b/c.txt" → ["a", "b", "c.txt"]）
    static const std::vector<std::string> getPathSegments(
        const std::filesystem::path& file_path);

    /// @brief 提取文件的三层扩展名（如 "a.component.json" → (".json", ".component", ".a")）
    static const std::tuple<std::string, std::string, std::string>
    getFileExtensions(const std::filesystem::path& file_path);

    /// @brief 去掉扩展名，只留纯文件名（如 "foo.component.json" → "foo"）
    static const std::string getFilePureName(const std::string file_full_name);
};

} // namespace Crumb
