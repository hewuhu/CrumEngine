module crumb.resource;
import :asset_manager;

import std;

namespace Crumb {

std::filesystem::path AssetManager::getFullPath(const std::string& relative_path) const {
    return std::filesystem::absolute(m_root_folder / relative_path);
}

} // namespace Crumb
