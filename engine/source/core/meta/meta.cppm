module;
#include "core/meta/reflection_macros.h"
#include <json11.hpp>

export module crumb.core.meta;

import std;
// 注：此模块不 import crumb.core（避免循环依赖）
// 只依赖 std + json11.hpp + reflection_macros.h（宏）

// ============================================================================
// crumb.core.meta — 反射运行时
// 提供 TypeMeta / FieldAccessor / Serializer 等运行时反射能力
// 移植自 Piccolo 引擎 reflection.h / serializer.h
// ============================================================================

export namespace Crumb {

// ============================================================================
// Json 类型别名
// ============================================================================
using Json = json11::Json;

// ============================================================================
// 函数签名 typedef
// ============================================================================

template<typename T, typename U, typename = void>
struct is_safely_castable : std::false_type {};

template<typename T, typename U>
struct is_safely_castable<T, U, std::void_t<decltype(static_cast<U>(std::declval<T>()))>> : std::true_type {};

namespace Reflection {
    class TypeMeta;
    class FieldAccessor;
    class MethodAccessor;
    class ArrayAccessor;
    class ReflectionInstance;
} // namespace Reflection

using SetFuncion    = std::function<void(void*, void*)>;
using GetFuncion    = std::function<void*(void*)>;
using GetNameFuncion = std::function<const char*()>;
using SetArrayFunc  = std::function<void(int, void*, void*)>;
using GetArrayFunc  = std::function<void*(int, void*)>;
using GetSizeFunc   = std::function<int(void*)>;
using GetBoolFunc   = std::function<bool()>;
using InvokeFunction = std::function<void(void*)>;

using ConstructorWithJson               = std::function<void*(const Json&)>;
using WriteJsonByName                   = std::function<Json(void*)>;
using GetBaseClassReflectionInstanceListFunc = std::function<int(Reflection::ReflectionInstance*&, void*)>;

using FieldFunctionTuple  = std::tuple<SetFuncion, GetFuncion, GetNameFuncion, GetNameFuncion, GetNameFuncion, GetBoolFunc>;
using MethodFunctionTuple = std::tuple<GetNameFuncion, InvokeFunction>;
using ClassFunctionTuple  = std::tuple<GetBaseClassReflectionInstanceListFunc, ConstructorWithJson, WriteJsonByName>;
using ArrayFunctionTuple  = std::tuple<SetArrayFunc, GetArrayFunc, GetSizeFunc, GetNameFuncion, GetNameFuncion>;

// ============================================================================
// Reflection 运行时类型
// ============================================================================

namespace Reflection {

// ----------------------------------------------------------------------------
// TypeMetaRegisterinterface — 全局注册表（静态方法）
// ----------------------------------------------------------------------------
class TypeMetaRegisterinterface {
public:
    static void registerToClassMap(const char* name, ClassFunctionTuple* value);
    static void registerToFieldMap(const char* name, FieldFunctionTuple* value);
    static void registerToMethodMap(const char* name, MethodFunctionTuple* value);
    static void registerToArrayMap(const char* name, ArrayFunctionTuple* value);
    static void unregisterAll();
};

// ----------------------------------------------------------------------------
// TypeMeta — 类型元数据
// ----------------------------------------------------------------------------
class TypeMeta {
    friend class FieldAccessor;
    friend class ArrayAccessor;
    friend class TypeMetaRegisterinterface;

public:
    TypeMeta();

    static TypeMeta newMetaFromName(std::string type_name);
    static bool     newArrayAccessorFromName(std::string array_type_name, ArrayAccessor& accessor);
    static ReflectionInstance newFromNameAndJson(std::string type_name, const Json& json_context);
    static Json     writeByName(std::string type_name, void* instance);

    std::string getTypeName();
    int getFieldsList(FieldAccessor*& out_list);
    int getMethodsList(MethodAccessor*& out_list);
    int getBaseClassReflectionInstanceList(ReflectionInstance*& out_list, void* instance);

    FieldAccessor getFieldByName(const char* name);
    MethodAccessor getMethodByName(const char* name);

    bool isValid() { return m_is_valid; }

    TypeMeta& operator=(const TypeMeta& dest);

private:
    TypeMeta(std::string type_name);

private:
    std::vector<FieldAccessor>  m_fields;
    std::vector<MethodAccessor> m_methods;
    std::string                 m_type_name;
    bool m_is_valid;
};

// ----------------------------------------------------------------------------
// FieldAccessor — 字段访问器
// ----------------------------------------------------------------------------
class FieldAccessor {
    friend class TypeMeta;

public:
    FieldAccessor();

    void* get(void* instance);
    void  set(void* instance, void* value);

    TypeMeta getOwnerTypeMeta();

    bool        getTypeMeta(TypeMeta& field_type);
    const char* getFieldName() const;
    const char* getFieldTypeName();
    bool        isArrayType();

    FieldAccessor& operator=(const FieldAccessor& dest);

private:
    FieldAccessor(FieldFunctionTuple* functions);

private:
    FieldFunctionTuple* m_functions;
    const char*         m_field_name;
    const char*         m_field_type_name;
};

// ----------------------------------------------------------------------------
// MethodAccessor — 方法访问器
// ----------------------------------------------------------------------------
class MethodAccessor {
    friend class TypeMeta;

public:
    MethodAccessor();

    void invoke(void* instance);
    const char* getMethodName() const;

    MethodAccessor& operator=(const MethodAccessor& dest);

private:
    MethodAccessor(MethodFunctionTuple* functions);

private:
    MethodFunctionTuple* m_functions;
    const char*          m_method_name;
};

// ----------------------------------------------------------------------------
// ArrayAccessor — std::vector 访问器
// ----------------------------------------------------------------------------
class ArrayAccessor {
    friend class TypeMeta;

public:
    ArrayAccessor();
    const char* getArrayTypeName();
    const char* getElementTypeName();
    void        set(int index, void* instance, void* element_value);
    void*       get(int index, void* instance);
    int         getSize(void* instance);

    ArrayAccessor& operator=(ArrayAccessor& dest);

private:
    ArrayAccessor(ArrayFunctionTuple* array_func);

private:
    ArrayFunctionTuple* m_func;
    const char*         m_array_type_name;
    const char*         m_element_type_name;
};

// ----------------------------------------------------------------------------
// ReflectionInstance — 反射实例（类型 + 指针 pair）
// ----------------------------------------------------------------------------
class ReflectionInstance {
public:
    ReflectionInstance(TypeMeta meta, void* instance) : m_meta(meta), m_instance(instance) {}
    ReflectionInstance() : m_meta(), m_instance(nullptr) {}

    ReflectionInstance& operator=(ReflectionInstance& dest);
    ReflectionInstance& operator=(ReflectionInstance&& dest);

public:
    TypeMeta m_meta;
    void*    m_instance;
};

// ----------------------------------------------------------------------------
// ReflectionPtr<T> — 带类型名的智能指针（多态序列化关键）
// ----------------------------------------------------------------------------
template<typename T>
class ReflectionPtr {
    template<typename U>
    friend class ReflectionPtr;

public:
    ReflectionPtr(std::string type_name, T* instance) : m_type_name(type_name), m_instance(instance) {}
    ReflectionPtr() : m_type_name(), m_instance(nullptr) {}

    ReflectionPtr(const ReflectionPtr& dest) : m_type_name(dest.m_type_name), m_instance(dest.m_instance) {}

    template<typename U>
    ReflectionPtr<T>& operator=(const ReflectionPtr<U>& dest) {
        if (this == static_cast<void*>(&dest)) return *this;
        m_type_name = dest.m_type_name;
        m_instance  = static_cast<T*>(dest.m_instance);
        return *this;
    }

    template<typename U>
    ReflectionPtr<T>& operator=(ReflectionPtr<U>&& dest) {
        if (this == static_cast<void*>(&dest)) return *this;
        m_type_name = dest.m_type_name;
        m_instance  = static_cast<T*>(dest.m_instance);
        return *this;
    }

    ReflectionPtr<T>& operator=(const ReflectionPtr<T>& dest) {
        if (this == &dest) return *this;
        m_type_name = dest.m_type_name;
        m_instance  = dest.m_instance;
        return *this;
    }

    ReflectionPtr<T>& operator=(ReflectionPtr<T>&& dest) {
        if (this == &dest) return *this;
        m_type_name = dest.m_type_name;
        m_instance  = dest.m_instance;
        return *this;
    }

    std::string getTypeName() const { return m_type_name; }
    void setTypeName(std::string name) { m_type_name = name; }

    bool operator==(const T* ptr) const { return (m_instance == ptr); }
    bool operator!=(const T* ptr) const { return (m_instance != ptr); }
    bool operator==(const ReflectionPtr<T>& rhs_ptr) const { return (m_instance == rhs_ptr.m_instance); }
    bool operator!=(const ReflectionPtr<T>& rhs_ptr) const { return (m_instance != rhs_ptr.m_instance); }

    template<typename T1>
    explicit operator T1*() { return static_cast<T1*>(m_instance); }

    template<typename T1>
    operator ReflectionPtr<T1>() { return ReflectionPtr<T1>(m_type_name, (T1*)(m_instance)); }

    template<typename T1>
    explicit operator const T1*() const { return static_cast<T1*>(m_instance); }

    template<typename T1>
    operator const ReflectionPtr<T1>() const { return ReflectionPtr<T1>(m_type_name, (T1*)(m_instance)); }

    T* operator->() { return m_instance; }
    T* operator->() const { return m_instance; }
    T& operator*() { return *(m_instance); }
    T* getPtr() { return m_instance; }
    T* getPtr() const { return m_instance; }
    const T& operator*() const { return *(static_cast<const T*>(m_instance)); }
    T*& getPtrReference() { return m_instance; }

    operator bool() const { return (m_instance != nullptr); }

private:
    std::string m_type_name {""};
    using m_type = T;
    T* m_instance {nullptr};
};

} // namespace Reflection

// ============================================================================
// Serializer — 泛型序列化器
// ============================================================================

template<typename...>
inline constexpr bool always_false = false;

class Serializer {
public:
    template<typename T>
    static Json writePointer(T* instance) {
        return Json::object {{"$typeName", Json {"*"}}, {"$context", Serializer::write(*instance)}};
    }

    template<typename T>
    static T*& readPointer(const Json& json_context, T*& instance) {
        std::string type_name = json_context["$typeName"].string_value();
        if (type_name.empty()) return instance;
        if ('*' == type_name[0]) {
            instance = new T;
            read(json_context["$context"], *instance);
        } else {
            instance = static_cast<T*>(
                Reflection::TypeMeta::newFromNameAndJson(type_name, json_context["$context"]).m_instance);
        }
        return instance;
    }

    template<typename T>
    static Json write(const Reflection::ReflectionPtr<T>& instance) {
        T*          instance_ptr = static_cast<T*>(instance.operator->());
        std::string type_name    = instance.getTypeName();
        return Json::object {{"$typeName", Json(type_name)},
                              {"$context", Reflection::TypeMeta::writeByName(type_name, instance_ptr)}};
    }

    template<typename T>
    static T*& read(const Json& json_context, Reflection::ReflectionPtr<T>& instance) {
        std::string type_name = json_context["$typeName"].string_value();
        instance.setTypeName(type_name);
        return readPointer(json_context, instance.getPtrReference());
    }

    template<typename T>
    static Json write(const T& instance) {
        if constexpr (std::is_pointer<T>::value) {
            return writePointer((T)instance);
        } else {
            static_assert(always_false<T>, "Serializer::write<T> has not been implemented yet!");
            return Json();
        }
    }

    template<typename T>
    static T& read(const Json& json_context, T& instance) {
        if constexpr (std::is_pointer<T>::value) {
            return readPointer(json_context, instance);
        } else {
            static_assert(always_false<T>, "Serializer::read<T> has not been implemented yet!");
            return instance;
        }
    }
};

// ============================================================================
// 基础类型显式特化声明
// ============================================================================
template<> Json Serializer::write(const char& instance);
template<> char& Serializer::read(const Json& json_context, char& instance);

template<> Json Serializer::write(const int& instance);
template<> int& Serializer::read(const Json& json_context, int& instance);

template<> Json Serializer::write(const unsigned int& instance);
template<> unsigned int& Serializer::read(const Json& json_context, unsigned int& instance);

template<> Json Serializer::write(const float& instance);
template<> float& Serializer::read(const Json& json_context, float& instance);

template<> Json Serializer::write(const double& instance);
template<> double& Serializer::read(const Json& json_context, double& instance);

template<> Json Serializer::write(const bool& instance);
template<> bool& Serializer::read(const Json& json_context, bool& instance);

template<> Json Serializer::write(const std::string& instance);
template<> std::string& Serializer::read(const Json& json_context, std::string& instance);

} // namespace Crumb
