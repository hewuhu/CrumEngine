module crumb.resource;
import :config_manager;

import std;

// ============================================================================
// ConfigManager 实现
// ============================================================================

namespace Crumb {

void ConfigManager::initialize(const std::filesystem::path& config_file_path) {
    std::ifstream config_file(config_file_path);
    std::string   config_line;
    while (std::getline(config_file, config_line)) {
        std::size_t separate_pos = config_line.find_first_of('=');
        if (separate_pos > 0 && separate_pos < (config_line.length() - 1)) {
            std::string name  = config_line.substr(0, separate_pos);
            std::string value = config_line.substr(separate_pos + 1, config_line.length() - separate_pos - 1);
            if (name == "BinaryRootFolder") {
                m_root_folder = std::filesystem::weakly_canonical(
                    config_file_path.parent_path() / value);
            } else if (name == "AssetFolder") {
                m_asset_folder = std::filesystem::weakly_canonical(
                    m_root_folder / value);
            } else if (name == "SchemaFolder") {
                m_schema_folder = std::filesystem::weakly_canonical(
                    m_root_folder / value);
            } else if (name == "DefaultWorld") {
                m_default_world_url = value;
            } else if (name == "GlobalRenderingRes") {
                m_global_rendering_res_url = value;
            } else if (name == "GlobalParticleRes") {
                m_global_particle_res_url = value;
            }
        }
    }
}

} // namespace Crumb
