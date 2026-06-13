export module crumb.resource:config_manager;

import std;

// ============================================================================
// ConfigManager — 引擎配置解析（key=value 文本格式）
// 移植自 Piccolo ConfigManager，适配 Crumb 模块体系
// ============================================================================

export namespace Crumb {

class ConfigManager {
public:
    void initialize(const std::filesystem::path& config_file_path);

    const std::filesystem::path& getRootFolder() const { return m_root_folder; }
    const std::filesystem::path& getAssetFolder() const { return m_asset_folder; }
    const std::filesystem::path& getSchemaFolder() const { return m_schema_folder; }

    const std::string& getDefaultWorldUrl() const { return m_default_world_url; }
    const std::string& getGlobalRenderingResUrl() const { return m_global_rendering_res_url; }
    const std::string& getGlobalParticleResUrl() const { return m_global_particle_res_url; }

private:
    std::filesystem::path m_root_folder;
    std::filesystem::path m_asset_folder;
    std::filesystem::path m_schema_folder;

    std::string m_default_world_url;
    std::string m_global_rendering_res_url;
    std::string m_global_particle_res_url;
};

} // namespace Crumb
