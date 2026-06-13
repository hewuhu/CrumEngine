export module crumb.resource:asset_manager;

import crumb.core.meta;
import std;

// ============================================================================
// AssetManager — 泛型资源加载 / 保存
// 模板方法通过 Serializer + Json 实现类型无关的 JSON ↔ C++ 序列化
// ============================================================================

export namespace Crumb {

class AssetManager {
public:
    /// @brief 设置根目录（由 ConfigManager::getRootFolder 提供）
    void setRootFolder(const std::filesystem::path& root) { m_root_folder = root; }

    /// @brief 从 JSON 文件加载资源（模板方法，依赖 Serializer<T> 特化）
    template<typename AssetType>
    bool loadAsset(const std::string& asset_url, AssetType& out_asset) const {
        std::filesystem::path asset_path = getFullPath(asset_url);
        std::ifstream asset_json_file(asset_path);
        if (!asset_json_file) return false;

        std::stringstream buffer;
        buffer << asset_json_file.rdbuf();
        std::string asset_json_text(buffer.str());

        std::string error;
        auto&&      asset_json = Json::parse(asset_json_text, error);
        if (!error.empty()) return false;

        Serializer::read(asset_json, out_asset);
        return true;
    }

    /// @brief 保存资源到 JSON 文件（模板方法）
    template<typename AssetType>
    bool saveAsset(const AssetType& out_asset, const std::string& asset_url) const {
        std::ofstream asset_json_file(getFullPath(asset_url));
        if (!asset_json_file) return false;

        auto&&        asset_json      = Serializer::write(out_asset);
        std::string&& asset_json_text = asset_json.dump();

        asset_json_file << asset_json_text;
        asset_json_file.flush();
        return true;
    }

    /// @brief 将相对路径拼接为绝对路径
    std::filesystem::path getFullPath(const std::string& relative_path) const;

private:
    std::filesystem::path m_root_folder;
};

// ============================================================================
// 通用文件 I/O 工具（不依赖 Json / Serializer）
// ============================================================================

/// @brief 读取整个文件到字节缓冲区（用于 shader SPIRV 等二进制文件）
inline std::vector<char> ReadFileBinary(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) return {};
    std::size_t size = file.tellg();
    file.seekg(0);
    std::vector<char> buffer(size / sizeof(char));
    file.read(buffer.data(), size);
    return buffer;
}

} // namespace Crumb
