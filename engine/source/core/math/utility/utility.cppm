export module crumb.core.math:utility;

import std;
import glm;

import :angle;
import :vector;
import :matrix;

export namespace Crumb {

class Quaternion;  // 前向声明

// ═══════════════════════════════════════════════════════════════
// Math — 静态数学工具类
// ═══════════════════════════════════════════════════════════════
class Math {
public:
    Math() = delete;

    // ── 常量 ──
    static constexpr float PI            = 3.14159265358979323846f;
    static constexpr float ONE_OVER_PI   = 1.0f / PI;
    static constexpr float TWO_PI        = 2.0f * PI;
    static constexpr float HALF_PI       = 0.5f * PI;
    static constexpr float DEG2RAD       = PI / 180.0f;
    static constexpr float RAD2DEG       = 180.0f / PI;
    static constexpr float LOG2          = 0.69314718055994530942f;
    static constexpr float EPSILON       = 1e-6f;
    static constexpr float POS_INFINITY  = std::numeric_limits<float>::infinity();
    static constexpr float NEG_INFINITY  = -std::numeric_limits<float>::infinity();

    // ── 基本数学 ──
    static float abs(float v) noexcept { return std::fabs(v); }
    static bool  isNan(float v) noexcept { return std::isnan(v); }
    static float sqr(float v) noexcept { return v * v; }
    static float sqrt(float v) noexcept { return std::sqrt(v); }
    static float invSqrt(float v) noexcept { return 1.0f / std::sqrt(v); }
    static float clamp(float v, float min, float max) noexcept { return std::clamp(v, min, max); }
    static float floor(float v) noexcept { return std::floor(v); }
    static float ceil(float v) noexcept { return std::ceil(v); }
    static float pow(float base, float exp) noexcept { return std::pow(base, exp); }
    static float exp(float v) noexcept { return std::exp(v); }
    static float log(float v) noexcept { return std::log(v); }

    template<typename T>
    static constexpr T max(T a, T b) noexcept { return a > b ? a : b; }

    template<typename T>
    static constexpr T min(T a, T b) noexcept { return a < b ? a : b; }

    template<typename T>
    static constexpr T max3(T a, T b, T c) noexcept { return max(max(a, b), c); }

    template<typename T>
    static constexpr T min3(T a, T b, T c) noexcept { return min(min(a, b), c); }

    static float getMaxElement(float x, float y, float z) noexcept { return max3(x, y, z); }

    static bool realEqual(float a, float b, float tolerance = std::numeric_limits<float>::epsilon()) noexcept {
        return std::fabs(b - a) <= tolerance;
    }

    // ── 角度转换 ──
    static float degreesToRadians(float degrees) noexcept { return degrees * DEG2RAD; }
    static float radiansToDegrees(float radians) noexcept { return radians * RAD2DEG; }

    // ── 三角函数 ──
    static float sin(float v) noexcept { return std::sin(v); }
    static float cos(float v) noexcept { return std::cos(v); }
    static float tan(float v) noexcept { return std::tan(v); }

    static Radian acos(float v) noexcept {
        return Radian(std::acos(std::clamp(v, -1.0f, 1.0f)));
    }
    static Radian asin(float v) noexcept {
        return Radian(std::asin(std::clamp(v, -1.0f, 1.0f)));
    }
    static Radian atan(float v) noexcept { return Radian(std::atan(v)); }
    static Radian atan2(float y, float x) noexcept { return Radian(std::atan2(y, x)); }

    // ── 矩阵构造 ──
    // makeViewMatrix 需要完整 Quaternion 定义，暂不实现
    // static Matrix4x4 makeViewMatrix(const Vector3& position, const Quaternion& orientation,
    //                                  const Matrix4x4* reflectMatrix = nullptr) noexcept;

    static Matrix4x4 makeLookAtMatrix(const Vector3& eyePosition, const Vector3& targetPosition,
                                        const Vector3& upDirection) noexcept {
        return Matrix4x4(glm::lookAt(eyePosition.toGlm(), targetPosition.toGlm(), upDirection.toGlm()));
    }

    static Matrix4x4 makePerspectiveMatrix(Radian fovy, float aspect, float znear, float zfar) noexcept {
        return Matrix4x4(glm::perspective(fovy.valueRadians(), aspect, znear, zfar));
    }

    static Matrix4x4 makeOrthographicProjectionMatrix(float left, float right, float bottom, float top,
                                                        float znear, float zfar) noexcept {
        return Matrix4x4(glm::ortho(left, right, bottom, top, znear, zfar));
    }

    static Matrix4x4 makeOrthographicProjectionMatrix01(float left, float right, float bottom, float top,
                                                          float znear, float zfar) noexcept {
        return Matrix4x4(glm::ortho(left, right, bottom, top, znear, zfar));
    }
};

// ═══════════════════════════════════════════════════════════════
// Random — 随机数生成器
// ═══════════════════════════════════════════════════════════════
template<typename NumericType>
using uniform_distribution_t = typename std::conditional<
    std::is_integral<NumericType>::value,
    std::uniform_int_distribution<NumericType>,
    std::uniform_real_distribution<NumericType>
>::type;

template<typename RandomEngine = std::default_random_engine>
class RandomNumberGenerator {
public:
    template<typename... Params>
    explicit RandomNumberGenerator(Params&&... params) noexcept
        : m_Engine(std::forward<Params>(params)...) {}

    template<typename... Params>
    void seed(Params&&... seeding) noexcept {
        m_Engine.seed(std::forward<Params>(seeding)...);
    }

    template<typename DistributionFunc, typename... Params>
    typename DistributionFunc::result_type distribution(Params&&... params) noexcept {
        DistributionFunc dist(std::forward<Params>(params)...);
        return dist(m_Engine);
    }

    template<typename NumericType>
    NumericType uniformDistribution(NumericType lower, NumericType upper) noexcept {
        if (lower == upper) return lower;
        return distribution<uniform_distribution_t<NumericType>>(lower, upper);
    }

    float uniformUnit() noexcept {
        return uniformDistribution(0.0f, std::nextafter(1.0f, std::numeric_limits<float>::max()));
    }

    float uniformSymmetry() noexcept {
        return uniformDistribution(-1.0f, std::nextafter(1.0f, std::numeric_limits<float>::max()));
    }

    bool bernoulliDistribution(float probability) noexcept {
        return distribution<std::bernoulli_distribution>(probability);
    }

    float normalDistribution(float mean, float stddev) noexcept {
        return distribution<std::normal_distribution<float>>(mean, stddev);
    }

    template<typename DistributionFunc, typename Range, typename... Params>
    void generator(Range&& range, Params&&... params) noexcept {
        DistributionFunc dist(std::forward<Params>(params)...);
        std::generate(std::begin(range), std::end(range), [&] { return dist(m_Engine); });
    }

private:
    RandomEngine m_Engine;
};

using DefaultRNG = RandomNumberGenerator<std::mt19937>;

} // namespace Crumb
