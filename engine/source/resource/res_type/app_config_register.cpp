// ============================================================================
// AppConfig 反射注册 — 手动编写（后续可改为代码生成）
// 注册到 crumb.core.meta 反射系统，打通 Serializer ↔ AssetManager
// ============================================================================

module;
#include "core/meta/reflection_macros.h"

module crumb.resource;
import :app_config;
import crumb.core.meta;
import std;

namespace Crumb {
namespace Reflection {
namespace TypeFieldReflectionOparator {

// ---------------------------------------------------------------------------
// TypeAppConfigOperator — 为每个字段提供 get/set/name/type/isArray
// ---------------------------------------------------------------------------
class TypeAppConfigOperator {
public:
    // --- rootFolder (std::string) ---
    static void set_rootFolder(void* instance, void* value) {
        static_cast<AppConfig*>(instance)->rootFolder = *static_cast<std::string*>(value);
    }
    static void* get_rootFolder(void* instance) {
        return &static_cast<AppConfig*>(instance)->rootFolder;
    }
    static const char* getFieldName_rootFolder() { return "rootFolder"; }
    static const char* getFieldTypeName_rootFolder() { return "std::string"; }
    static bool isArray_rootFolder() { return false; }

    // --- assetFolder (std::string) ---
    static void set_assetFolder(void* instance, void* value) {
        static_cast<AppConfig*>(instance)->assetFolder = *static_cast<std::string*>(value);
    }
    static void* get_assetFolder(void* instance) {
        return &static_cast<AppConfig*>(instance)->assetFolder;
    }
    static const char* getFieldName_assetFolder() { return "assetFolder"; }
    static const char* getFieldTypeName_assetFolder() { return "std::string"; }
    static bool isArray_assetFolder() { return false; }

    // --- defaultWorldUrl (std::string) ---
    static void set_defaultWorldUrl(void* instance, void* value) {
        static_cast<AppConfig*>(instance)->defaultWorldUrl = *static_cast<std::string*>(value);
    }
    static void* get_defaultWorldUrl(void* instance) {
        return &static_cast<AppConfig*>(instance)->defaultWorldUrl;
    }
    static const char* getFieldName_defaultWorldUrl() { return "defaultWorldUrl"; }
    static const char* getFieldTypeName_defaultWorldUrl() { return "std::string"; }
    static bool isArray_defaultWorldUrl() { return false; }

    // --- globalRenderingResUrl (std::string) ---
    static void set_globalRenderingResUrl(void* instance, void* value) {
        static_cast<AppConfig*>(instance)->globalRenderingResUrl = *static_cast<std::string*>(value);
    }
    static void* get_globalRenderingResUrl(void* instance) {
        return &static_cast<AppConfig*>(instance)->globalRenderingResUrl;
    }
    static const char* getFieldName_globalRenderingResUrl() { return "globalRenderingResUrl"; }
    static const char* getFieldTypeName_globalRenderingResUrl() { return "std::string"; }
    static bool isArray_globalRenderingResUrl() { return false; }

    // --- windowWidth (int) ---
    static void set_windowWidth(void* instance, void* value) {
        static_cast<AppConfig*>(instance)->windowWidth = *static_cast<int*>(value);
    }
    static void* get_windowWidth(void* instance) {
        return &static_cast<AppConfig*>(instance)->windowWidth;
    }
    static const char* getFieldName_windowWidth() { return "windowWidth"; }
    static const char* getFieldTypeName_windowWidth() { return "int"; }
    static bool isArray_windowWidth() { return false; }

    // --- windowHeight (int) ---
    static void set_windowHeight(void* instance, void* value) {
        static_cast<AppConfig*>(instance)->windowHeight = *static_cast<int*>(value);
    }
    static void* get_windowHeight(void* instance) {
        return &static_cast<AppConfig*>(instance)->windowHeight;
    }
    static const char* getFieldName_windowHeight() { return "windowHeight"; }
    static const char* getFieldTypeName_windowHeight() { return "int"; }
    static bool isArray_windowHeight() { return false; }

    // --- 基类 + JSON 构造 ---
    static int getBaseList(ReflectionInstance*& out, void*) { out = nullptr; return 0; }
    static void* constructorWithJson(const Json& json) {
        auto* cfg = new AppConfig();
        Serializer::read(json, *cfg);
        return cfg;
    }
    static Json writeJsonByName(void* instance) {
        return Serializer::write(*static_cast<AppConfig*>(instance));
    }
};

} // namespace TypeFieldReflectionOparator
} // namespace Reflection
} // namespace Crumb

// ============================================================================
// 自动注册函数（在程序启动时调用一次）
// ============================================================================

namespace {

using namespace Crumb;
using namespace Crumb::Reflection;
using namespace Crumb::Reflection::TypeFieldReflectionOparator;

// RAII 注册辅助
struct AutoRegister {
    AutoRegister() {
        // Class (constructor + writeByName + base list)
        auto* classTuple = new ClassFunctionTuple(
            TypeAppConfigOperator::getBaseList,
            TypeAppConfigOperator::constructorWithJson,
            TypeAppConfigOperator::writeJsonByName);
        REGISTER_BASE_CLASS_TO_MAP("AppConfig", classTuple);

        // Fields
        REGISTER_FIELD_TO_MAP("AppConfig", new FieldFunctionTuple(
            TypeAppConfigOperator::set_rootFolder,
            TypeAppConfigOperator::get_rootFolder,
            TypeAppConfigOperator::getFieldTypeName_rootFolder,
            TypeAppConfigOperator::getFieldName_rootFolder,
            TypeAppConfigOperator::getFieldTypeName_rootFolder,
            TypeAppConfigOperator::isArray_rootFolder));

        REGISTER_FIELD_TO_MAP("AppConfig", new FieldFunctionTuple(
            TypeAppConfigOperator::set_assetFolder,
            TypeAppConfigOperator::get_assetFolder,
            TypeAppConfigOperator::getFieldTypeName_assetFolder,
            TypeAppConfigOperator::getFieldName_assetFolder,
            TypeAppConfigOperator::getFieldTypeName_assetFolder,
            TypeAppConfigOperator::isArray_assetFolder));

        REGISTER_FIELD_TO_MAP("AppConfig", new FieldFunctionTuple(
            TypeAppConfigOperator::set_defaultWorldUrl,
            TypeAppConfigOperator::get_defaultWorldUrl,
            TypeAppConfigOperator::getFieldTypeName_defaultWorldUrl,
            TypeAppConfigOperator::getFieldName_defaultWorldUrl,
            TypeAppConfigOperator::getFieldTypeName_defaultWorldUrl,
            TypeAppConfigOperator::isArray_defaultWorldUrl));

        REGISTER_FIELD_TO_MAP("AppConfig", new FieldFunctionTuple(
            TypeAppConfigOperator::set_globalRenderingResUrl,
            TypeAppConfigOperator::get_globalRenderingResUrl,
            TypeAppConfigOperator::getFieldTypeName_globalRenderingResUrl,
            TypeAppConfigOperator::getFieldName_globalRenderingResUrl,
            TypeAppConfigOperator::getFieldTypeName_globalRenderingResUrl,
            TypeAppConfigOperator::isArray_globalRenderingResUrl));

        REGISTER_FIELD_TO_MAP("AppConfig", new FieldFunctionTuple(
            TypeAppConfigOperator::set_windowWidth,
            TypeAppConfigOperator::get_windowWidth,
            TypeAppConfigOperator::getFieldTypeName_windowWidth,
            TypeAppConfigOperator::getFieldName_windowWidth,
            TypeAppConfigOperator::getFieldTypeName_windowWidth,
            TypeAppConfigOperator::isArray_windowWidth));

        REGISTER_FIELD_TO_MAP("AppConfig", new FieldFunctionTuple(
            TypeAppConfigOperator::set_windowHeight,
            TypeAppConfigOperator::get_windowHeight,
            TypeAppConfigOperator::getFieldTypeName_windowHeight,
            TypeAppConfigOperator::getFieldName_windowHeight,
            TypeAppConfigOperator::getFieldTypeName_windowHeight,
            TypeAppConfigOperator::isArray_windowHeight));
    }
};

static AutoRegister s_registerAppConfig;

} // anonymous namespace
