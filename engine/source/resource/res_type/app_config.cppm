module;
#include "core/meta/reflection_macros.h"

export module crumb.resource:app_config;

import crumb.core.meta;
import std;

// ============================================================================
// AppConfig — 引擎配置资源类型
// 第一个反射注册类型，验证 Serializer + AssetManager 全链路
// ============================================================================

export namespace Crumb {

REFLECTION_TYPE(AppConfig)
CLASS(AppConfig, Fields)
{
    REFLECTION_BODY(AppConfig);

public:
    std::string rootFolder;
    std::string assetFolder;
    std::string defaultWorldUrl;
    std::string globalRenderingResUrl;

    int windowWidth  = 1280;
    int windowHeight = 720;
};

// Serializer 特化声明（定义在 app_config_serializer.cpp）
template<> Json        Serializer::write(const AppConfig& instance);
template<> AppConfig&  Serializer::read(const Json& json, AppConfig& instance);

} // namespace Crumb
