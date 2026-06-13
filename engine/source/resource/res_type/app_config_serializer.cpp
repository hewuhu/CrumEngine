// ============================================================================
// AppConfig Serializer 显式特化
// 使 AssetManager::loadAsset<AppConfig>() / saveAsset<AppConfig>() 可用
// ============================================================================

module crumb.resource;
import :app_config;

import crumb.core.meta;
import std;

namespace Crumb {

template<>
Json Serializer::write(const AppConfig& instance) {
    return Json::object{
        {"rootFolder",            Json(instance.rootFolder)},
        {"assetFolder",           Json(instance.assetFolder)},
        {"defaultWorldUrl",       Json(instance.defaultWorldUrl)},
        {"globalRenderingResUrl", Json(instance.globalRenderingResUrl)},
        {"windowWidth",           Json(instance.windowWidth)},
        {"windowHeight",          Json(instance.windowHeight)},
    };
}

template<>
AppConfig& Serializer::read(const Json& json, AppConfig& instance) {
    if (json.is_object()) {
        if (json["rootFolder"].is_string())
            instance.rootFolder = json["rootFolder"].string_value();
        if (json["assetFolder"].is_string())
            instance.assetFolder = json["assetFolder"].string_value();
        if (json["defaultWorldUrl"].is_string())
            instance.defaultWorldUrl = json["defaultWorldUrl"].string_value();
        if (json["globalRenderingResUrl"].is_string())
            instance.globalRenderingResUrl = json["globalRenderingResUrl"].string_value();
        if (json["windowWidth"].is_number())
            instance.windowWidth = json["windowWidth"].int_value();
        if (json["windowHeight"].is_number())
            instance.windowHeight = json["windowHeight"].int_value();
    }
    return instance;
}

} // namespace Crumb
