export module crumb.core.math:vector;
import std;
import glm;
import :angle;

export namespace Crumb {

// ── 前向声明（解决循环依赖） ──
class Quaternion;

// ═══════════════════════════════════════════════════════════════
// Vector2
// ═══════════════════════════════════════════════════════════════
class Vector2 {
public:
    float x{0.f}, y{0.f};

    Vector2() = default;
    constexpr Vector2(float x_, float y_) noexcept : x(x_), y(y_) {}
    constexpr explicit Vector2(float scaler) noexcept : x(scaler), y(scaler) {}
    explicit Vector2(const float coords[2]) noexcept : x(coords[0]), y(coords[1]) {}
    explicit Vector2(float* const r) noexcept : x(r[0]), y(r[1]) {}
    explicit Vector2(const glm::vec2& v) noexcept : x(v.x), y(v.y) {}

    // ── 访问器 ──
    float*       ptr() noexcept { return &x; }
    const float* ptr() const noexcept { return &x; }

    float  operator[](std::size_t i) const noexcept { return (&x)[i]; }
    float& operator[](std::size_t i) noexcept { return (&x)[i]; }

    // ── GLM 互操作（内部使用） ──
    glm::vec2 toGlm() const noexcept { return glm::vec2(x, y); }
    static Vector2 fromGlm(const glm::vec2& v) noexcept { return Vector2(v.x, v.y); }

    // ── 长度 ──
    float length() const noexcept { return std::hypot(x, y); }
    float squaredLength() const noexcept { return x * x + y * y; }
    float distance(const Vector2& rhs) const noexcept { return (*this - rhs).length(); }
    float squaredDistance(const Vector2& rhs) const noexcept { return (*this - rhs).squaredLength(); }

    // ── 归一化 ──
    float normalise() noexcept {
        float len = length();
        if (len > 0.f) { x /= len; y /= len; }
        return len;
    }
    Vector2 normalisedCopy() const noexcept {
        Vector2 r = *this;
        r.normalise();
        return r;
    }

    // ── 乘积 ──
    float   dotProduct(const Vector2& rhs) const noexcept { return x * rhs.x + y * rhs.y; }
    float   crossProduct(const Vector2& rhs) const noexcept { return x * rhs.y - y * rhs.x; }

    // ── 反射 / 垂线 ──
    Vector2 reflect(const Vector2& normal) const noexcept {
        float d = this->dotProduct(normal);
        return Vector2(x - 2 * d * normal.x, y - 2 * d * normal.y);
    }
    Vector2 perpendicular() const noexcept { return Vector2(-y, x); }

    // ── 中点 ──
    Vector2 midPoint(const Vector2& vec) const noexcept {
        return Vector2((x + vec.x) * 0.5f, (y + vec.y) * 0.5f);
    }

    // ── 分量操作 ──
    void makeFloor(const Vector2& cmp) noexcept {
        if (cmp.x < x) x = cmp.x;
        if (cmp.y < y) y = cmp.y;
    }
    void makeCeil(const Vector2& cmp) noexcept {
        if (cmp.x > x) x = cmp.x;
        if (cmp.y > y) y = cmp.y;
    }
    Vector2 absoluteCopy() const noexcept { return Vector2(std::fabs(x), std::fabs(y)); }

    bool isZeroLength() const noexcept { return squaredLength() < 1e-12f; }
    bool isNaN() const noexcept { return std::isnan(x) || std::isnan(y); }

    // ── 插值 ──
    static Vector2 lerp(const Vector2& lhs, const Vector2& rhs, float alpha) noexcept {
        return lhs + (rhs - lhs) * alpha;
    }

    // ── 运算符 ──
    constexpr Vector2 operator+(const Vector2& rhs) const noexcept { return Vector2(x + rhs.x, y + rhs.y); }
    constexpr Vector2 operator-(const Vector2& rhs) const noexcept { return Vector2(x - rhs.x, y - rhs.y); }
    constexpr Vector2 operator*(float s) const noexcept { return Vector2(x * s, y * s); }
    constexpr Vector2 operator*(const Vector2& rhs) const noexcept { return Vector2(x * rhs.x, y * rhs.y); }
    constexpr Vector2 operator/(float s) const noexcept { return Vector2(x / s, y / s); }
    constexpr Vector2 operator/(const Vector2& rhs) const noexcept { return Vector2(x / rhs.x, y / rhs.y); }
    constexpr Vector2 operator-() const noexcept { return Vector2(-x, -y); }

    constexpr Vector2& operator+=(const Vector2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
    constexpr Vector2& operator-=(const Vector2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
    constexpr Vector2& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
    constexpr Vector2& operator*=(const Vector2& rhs) noexcept { x *= rhs.x; y *= rhs.y; return *this; }
    constexpr Vector2& operator/=(float s) noexcept { x /= s; y /= s; return *this; }
    constexpr Vector2& operator/=(const Vector2& rhs) noexcept { x /= rhs.x; y /= rhs.y; return *this; }

    constexpr bool operator==(const Vector2& rhs) const noexcept { return x == rhs.x && y == rhs.y; }
    constexpr bool operator!=(const Vector2& rhs) const noexcept { return x != rhs.x || y != rhs.y; }
    constexpr bool operator<(const Vector2& rhs) const noexcept { return x < rhs.x && y < rhs.y; }
    constexpr bool operator>(const Vector2& rhs) const noexcept { return x > rhs.x && y > rhs.y; }

    // ── 静态常量 ──
    static const Vector2 ZERO;
    static const Vector2 UNIT_X;
    static const Vector2 UNIT_Y;
    static const Vector2 NEGATIVE_UNIT_X;
    static const Vector2 NEGATIVE_UNIT_Y;
    static const Vector2 UNIT_SCALE;
};

inline constexpr Vector2 Vector2::ZERO(0, 0);
inline constexpr Vector2 Vector2::UNIT_X(1, 0);
inline constexpr Vector2 Vector2::UNIT_Y(0, 1);
inline constexpr Vector2 Vector2::NEGATIVE_UNIT_X(-1, 0);
inline constexpr Vector2 Vector2::NEGATIVE_UNIT_Y(0, -1);
inline constexpr Vector2 Vector2::UNIT_SCALE(1, 1);

// ── 自由运算符: 标量 * Vector2 ──
inline constexpr Vector2 operator*(float s, const Vector2& v) noexcept { return v * s; }
inline constexpr Vector2 operator/(float s, const Vector2& v) noexcept { return Vector2(s / v.x, s / v.y); }
inline constexpr Vector2 operator+(const Vector2& lhs, float rhs) noexcept { return Vector2(lhs.x + rhs, lhs.y + rhs); }
inline constexpr Vector2 operator+(float lhs, const Vector2& rhs) noexcept { return Vector2(lhs + rhs.x, lhs + rhs.y); }
inline constexpr Vector2 operator-(const Vector2& lhs, float rhs) noexcept { return Vector2(lhs.x - rhs, lhs.y - rhs); }
inline constexpr Vector2 operator-(float lhs, const Vector2& rhs) noexcept { return Vector2(lhs - rhs.x, lhs - rhs.y); }

// ═══════════════════════════════════════════════════════════════
// Vector3
// ═══════════════════════════════════════════════════════════════
class Vector3 {
public:
    float x{0.f}, y{0.f}, z{0.f};

    Vector3() = default;
    constexpr Vector3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}
    explicit Vector3(const float coords[3]) noexcept : x(coords[0]), y(coords[1]), z(coords[2]) {}
    explicit Vector3(const glm::vec3& v) noexcept : x(v.x), y(v.y), z(v.z) {}

    // ── 访问器 ──
    float*       ptr() noexcept { return &x; }
    const float* ptr() const noexcept { return &x; }

    float  operator[](std::size_t i) const noexcept { return (&x)[i]; }
    float& operator[](std::size_t i) noexcept { return (&x)[i]; }

    // ── GLM 互操作 ──
    glm::vec3 toGlm() const noexcept { return glm::vec3(x, y, z); }
    static Vector3 fromGlm(const glm::vec3& v) noexcept { return Vector3(v.x, v.y, v.z); }

    // ── 长度 ──
    float length() const noexcept { return std::hypot(x, y, z); }
    float squaredLength() const noexcept { return x * x + y * y + z * z; }
    float distance(const Vector3& rhs) const noexcept { return (*this - rhs).length(); }
    float squaredDistance(const Vector3& rhs) const noexcept { return (*this - rhs).squaredLength(); }

    // ── 归一化 ──
    void normalise() noexcept {
        float len = std::hypot(x, y, z);
        if (len > 0.f) { x /= len; y /= len; z /= len; }
    }
    Vector3 normalisedCopy() const noexcept {
        Vector3 r = *this;
        r.normalise();
        return r;
    }

    // ── 乘积 ──
    constexpr float   dotProduct(const Vector3& rhs) const noexcept { return x * rhs.x + y * rhs.y + z * rhs.z; }
    constexpr Vector3 crossProduct(const Vector3& rhs) const noexcept {
        return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
    }

    // ── 反射 / 投影 ──
    Vector3 reflect(const Vector3& normal) const noexcept {
        return Vector3(glm::reflect(toGlm(), normal.toGlm()));
    }
    Vector3 project(const Vector3& normal) const noexcept {
        float d = this->dotProduct(normal);
        return Vector3(x - d * normal.x, y - d * normal.y, z - d * normal.z);
    }

    // ── 角度 ──
    Radian angleBetween(const Vector3& dest) const noexcept {
        float lenProduct = length() * dest.length();
        if (lenProduct < 1e-6f) lenProduct = 1e-6f;
        float f = dotProduct(dest) / lenProduct;
        f = std::clamp(f, -1.0f, 1.0f);
        return Radian(std::acos(f));
    }

    // ── 旋转到目标向量 ──（体在 quaternion 分区定义）
    Quaternion getRotationTo(const Vector3& dest, const Vector3& fallbackAxis = Vector3::ZERO) const;

    // ── 分量操作 ──
    void makeFloor(const Vector3& cmp) noexcept {
        if (cmp.x < x) x = cmp.x;
        if (cmp.y < y) y = cmp.y;
        if (cmp.z < z) z = cmp.z;
    }
    void makeCeil(const Vector3& cmp) noexcept {
        if (cmp.x > x) x = cmp.x;
        if (cmp.y > y) y = cmp.y;
        if (cmp.z > z) z = cmp.z;
    }
    Vector3 absoluteCopy() const noexcept { return Vector3(std::fabs(x), std::fabs(y), std::fabs(z)); }

    bool isZeroLength() const noexcept { return squaredLength() < 1e-12f; }
    bool isZero() const noexcept { return x == 0.f && y == 0.f && z == 0.f; }
    bool isNaN() const noexcept { return std::isnan(x) || std::isnan(y) || std::isnan(z); }

    // ── 插值 / 钳制 ──
    static Vector3 lerp(const Vector3& lhs, const Vector3& rhs, float alpha) noexcept {
        return lhs + (rhs - lhs) * alpha;
    }
    static Vector3 clamp(const Vector3& v, const Vector3& min, const Vector3& max) noexcept {
        return Vector3(std::clamp(v.x, min.x, max.x),
                       std::clamp(v.y, min.y, max.y),
                       std::clamp(v.z, min.z, max.z));
    }
    static float getMaxElement(const Vector3& v) noexcept { return std::max({v.x, v.y, v.z}); }

    // ── 运算符 ──
    constexpr Vector3 operator+(const Vector3& rhs) const noexcept { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
    constexpr Vector3 operator-(const Vector3& rhs) const noexcept { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
    constexpr Vector3 operator*(float s) const noexcept { return Vector3(x * s, y * s, z * s); }
    constexpr Vector3 operator*(const Vector3& rhs) const noexcept { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }
    constexpr Vector3 operator/(float s) const noexcept { return Vector3(x / s, y / s, z / s); }
    constexpr Vector3 operator/(const Vector3& rhs) const noexcept { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }
    constexpr Vector3 operator-() const noexcept { return Vector3(-x, -y, -z); }

    constexpr Vector3& operator+=(const Vector3& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    constexpr Vector3& operator-=(const Vector3& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    constexpr Vector3& operator*=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }
    constexpr Vector3& operator*=(const Vector3& rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
    constexpr Vector3& operator/=(float s) noexcept { x /= s; y /= s; z /= s; return *this; }
    constexpr Vector3& operator/=(const Vector3& rhs) noexcept { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

    constexpr Vector3& operator+=(float s) noexcept { x += s; y += s; z += s; return *this; }
    constexpr Vector3& operator-=(float s) noexcept { x -= s; y -= s; z -= s; return *this; }

    constexpr bool operator==(const Vector3& rhs) const noexcept { return x == rhs.x && y == rhs.y && z == rhs.z; }
    constexpr bool operator!=(const Vector3& rhs) const noexcept { return x != rhs.x || y != rhs.y || z != rhs.z; }

    // ── 静态常量 ──
    static const Vector3 ZERO;
    static const Vector3 UNIT_X;
    static const Vector3 UNIT_Y;
    static const Vector3 UNIT_Z;
    static const Vector3 NEGATIVE_UNIT_X;
    static const Vector3 NEGATIVE_UNIT_Y;
    static const Vector3 NEGATIVE_UNIT_Z;
    static const Vector3 UNIT_SCALE;
};

inline constexpr Vector3 Vector3::ZERO(0, 0, 0);
inline constexpr Vector3 Vector3::UNIT_X(1, 0, 0);
inline constexpr Vector3 Vector3::UNIT_Y(0, 1, 0);
inline constexpr Vector3 Vector3::UNIT_Z(0, 0, 1);
inline constexpr Vector3 Vector3::NEGATIVE_UNIT_X(-1, 0, 0);
inline constexpr Vector3 Vector3::NEGATIVE_UNIT_Y(0, -1, 0);
inline constexpr Vector3 Vector3::NEGATIVE_UNIT_Z(0, 0, -1);
inline constexpr Vector3 Vector3::UNIT_SCALE(1, 1, 1);

// ── 自由运算符: 标量 * Vector3 ──
inline constexpr Vector3 operator*(float s, const Vector3& v) noexcept { return v * s; }
inline constexpr Vector3 operator/(float s, const Vector3& v) noexcept { return Vector3(s / v.x, s / v.y, s / v.z); }
inline constexpr Vector3 operator+(const Vector3& lhs, float rhs) noexcept { return Vector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs); }
inline constexpr Vector3 operator+(float lhs, const Vector3& rhs) noexcept { return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z); }
inline constexpr Vector3 operator-(const Vector3& lhs, float rhs) noexcept { return Vector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs); }
inline constexpr Vector3 operator-(float lhs, const Vector3& rhs) noexcept { return Vector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z); }

// ═══════════════════════════════════════════════════════════════
// Vector4
// ═══════════════════════════════════════════════════════════════
class Vector4 {
public:
    float x{0.f}, y{0.f}, z{0.f}, w{0.f};

    Vector4() = default;
    constexpr Vector4(float x_, float y_, float z_, float w_) noexcept : x(x_), y(y_), z(z_), w(w_) {}
    Vector4(const Vector3& v3, float w_) noexcept : x(v3.x), y(v3.y), z(v3.z), w(w_) {}
    explicit Vector4(const float coords[4]) noexcept : x(coords[0]), y(coords[1]), z(coords[2]), w(coords[3]) {}
    explicit Vector4(const glm::vec4& v) noexcept : x(v.x), y(v.y), z(v.z), w(v.w) {}

    // ── 访问器 ──
    float*       ptr() noexcept { return &x; }
    const float* ptr() const noexcept { return &x; }

    float  operator[](std::size_t i) const noexcept { return (&x)[i]; }
    float& operator[](std::size_t i) noexcept { return (&x)[i]; }

    // ── GLM 互操作 ──
    glm::vec4 toGlm() const noexcept { return glm::vec4(x, y, z, w); }
    static Vector4 fromGlm(const glm::vec4& v) noexcept { return Vector4(v.x, v.y, v.z, v.w); }

    // ── 到 Vector3（透视除法） ──
    Vector3 toVector3() const noexcept {
        if (w == 0.f) return Vector3::ZERO;
        float invW = 1.0f / w;
        return Vector3(x * invW, y * invW, z * invW);
    }

    // ── 乘积 ──
    constexpr float dotProduct(const Vector4& rhs) const noexcept {
        return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
    }

    bool isNaN() const noexcept { return std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w); }

    // ── 运算符 ──
    constexpr Vector4 operator+(const Vector4& rhs) const noexcept { return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
    constexpr Vector4 operator-(const Vector4& rhs) const noexcept { return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
    constexpr Vector4 operator*(float s) const noexcept { return Vector4(x * s, y * s, z * s, w * s); }
    constexpr Vector4 operator*(const Vector4& rhs) const noexcept { return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
    constexpr Vector4 operator/(float s) const noexcept { return Vector4(x / s, y / s, z / s, w / s); }
    constexpr Vector4 operator/(const Vector4& rhs) const noexcept { return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); }
    constexpr Vector4 operator-() const noexcept { return Vector4(-x, -y, -z, -w); }

    constexpr Vector4& operator+=(const Vector4& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
    constexpr Vector4& operator-=(const Vector4& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
    constexpr Vector4& operator*=(float s) noexcept { x *= s; y *= s; z *= s; w *= s; return *this; }
    constexpr Vector4& operator*=(const Vector4& rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
    constexpr Vector4& operator/=(float s) noexcept { x /= s; y /= s; z /= s; w /= s; return *this; }
    constexpr Vector4& operator/=(const Vector4& rhs) noexcept { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }

    constexpr bool operator==(const Vector4& rhs) const noexcept { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
    constexpr bool operator!=(const Vector4& rhs) const noexcept { return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w; }

    // ── 静态常量 ──
    static const Vector4 ZERO;
    static const Vector4 UNIT_SCALE;
};

inline constexpr Vector4 Vector4::ZERO(0, 0, 0, 0);
inline constexpr Vector4 Vector4::UNIT_SCALE(1, 1, 1, 1);

// ── 自由运算符: 标量 * Vector4 ──
inline constexpr Vector4 operator*(float s, const Vector4& v) noexcept { return v * s; }

} // namespace Crumb
