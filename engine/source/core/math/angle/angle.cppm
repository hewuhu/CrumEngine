export module crumb.core.math:angle;

import std;

export namespace Crumb {

// ── 数学常量 ──
inline constexpr float Math_PI          = 3.14159265358979323846f;
inline constexpr float Math_ONE_OVER_PI = 1.0f / Math_PI;
inline constexpr float Math_TWO_PI      = 2.0f * Math_PI;
inline constexpr float Math_HALF_PI     = 0.5f * Math_PI;
inline constexpr float Math_fDeg2Rad    = Math_PI / 180.0f;
inline constexpr float Math_fRad2Deg    = 180.0f / Math_PI;

// ── 前向声明 ──
class Radian;
class Degree;

// ═══════════════════════════════════════════════════════════════
// Radian — 弧度包装器
// ═══════════════════════════════════════════════════════════════
class Radian {
public:
    constexpr explicit Radian(float r = 0.0f) noexcept : m_Value(r) {}

    constexpr float valueRadians() const noexcept { return m_Value; }
    float           valueDegrees() const noexcept;  // Degree 定义后实现
    float           valueAngleUnits() const noexcept { return m_Value; }

    constexpr void setValue(float f) noexcept { m_Value = f; }

    // ── 运算符 ──
    constexpr const Radian& operator+() const noexcept { return *this; }
    constexpr Radian        operator-() const noexcept { return Radian(-m_Value); }

    constexpr Radian  operator+(const Radian& r) const noexcept { return Radian(m_Value + r.m_Value); }
    constexpr Radian  operator-(const Radian& r) const noexcept { return Radian(m_Value - r.m_Value); }
    constexpr Radian  operator*(float f) const noexcept { return Radian(m_Value * f); }
    constexpr Radian  operator*(const Radian& f) const noexcept { return Radian(m_Value * f.m_Value); }
    constexpr Radian  operator/(float f) const noexcept { return Radian(m_Value / f); }

    constexpr Radian& operator+=(const Radian& r) noexcept { m_Value += r.m_Value; return *this; }
    constexpr Radian& operator-=(const Radian& r) noexcept { m_Value -= r.m_Value; return *this; }
    constexpr Radian& operator*=(float f) noexcept { m_Value *= f; return *this; }
    constexpr Radian& operator/=(float f) noexcept { m_Value /= f; return *this; }

    // Degree 交互 — Degree 定义后实现
    Radian  operator+(const Degree& d) const noexcept;
    Radian  operator-(const Degree& d) const noexcept;
    Radian& operator+=(const Degree& d) noexcept;
    Radian& operator-=(const Degree& d) noexcept;

    // ── 比较 ──
    constexpr bool operator<(const Radian& r)  const noexcept { return m_Value <  r.m_Value; }
    constexpr bool operator<=(const Radian& r) const noexcept { return m_Value <= r.m_Value; }
    constexpr bool operator==(const Radian& r) const noexcept { return m_Value == r.m_Value; }
    constexpr bool operator!=(const Radian& r) const noexcept { return m_Value != r.m_Value; }
    constexpr bool operator>=(const Radian& r) const noexcept { return m_Value >= r.m_Value; }
    constexpr bool operator>(const Radian& r)  const noexcept { return m_Value >  r.m_Value; }

private:
    float m_Value;
};

// ═══════════════════════════════════════════════════════════════
// Degree — 角度包装器
// ═══════════════════════════════════════════════════════════════
class Degree {
public:
    constexpr explicit Degree(float d = 0.0f) noexcept : m_Value(d) {}
    explicit Degree(const Radian& r) noexcept : m_Value(r.valueDegrees()) {}

    constexpr float valueDegrees() const noexcept { return m_Value; }
    float           valueRadians() const noexcept;  // Radian 方法可用后实现
    float           valueAngleUnits() const noexcept { return m_Value; }

    constexpr void setValue(float f) noexcept { m_Value = f; }

    // ── 运算符 ──
    constexpr const Degree& operator+() const noexcept { return *this; }
    constexpr Degree        operator-() const noexcept { return Degree(-m_Value); }

    constexpr Degree  operator+(const Degree& d) const noexcept { return Degree(m_Value + d.m_Value); }
    constexpr Degree  operator-(const Degree& d) const noexcept { return Degree(m_Value - d.m_Value); }
    constexpr Degree  operator*(float f) const noexcept { return Degree(m_Value * f); }
    constexpr Degree  operator*(const Degree& f) const noexcept { return Degree(m_Value * f.m_Value); }
    constexpr Degree  operator/(float f) const noexcept { return Degree(m_Value / f); }

    constexpr Degree& operator+=(const Degree& d) noexcept { m_Value += d.m_Value; return *this; }
    constexpr Degree& operator-=(const Degree& d) noexcept { m_Value -= d.m_Value; return *this; }
    constexpr Degree& operator*=(float f) noexcept { m_Value *= f; return *this; }
    constexpr Degree& operator/=(float f) noexcept { m_Value /= f; return *this; }

    Degree  operator+(const Radian& r) const noexcept { return Degree(m_Value + r.valueDegrees()); }
    Degree  operator-(const Radian& r) const noexcept { return Degree(m_Value - r.valueDegrees()); }
    Degree& operator+=(const Radian& r) noexcept { m_Value += r.valueDegrees(); return *this; }
    Degree& operator-=(const Radian& r) noexcept { m_Value -= r.valueDegrees(); return *this; }

    Degree& operator=(const Radian& r) noexcept { m_Value = r.valueDegrees(); return *this; }

    // ── 比较 ──
    constexpr bool operator<(const Degree& d)  const noexcept { return m_Value <  d.m_Value; }
    constexpr bool operator<=(const Degree& d) const noexcept { return m_Value <= d.m_Value; }
    constexpr bool operator==(const Degree& d) const noexcept { return m_Value == d.m_Value; }
    constexpr bool operator!=(const Degree& d) const noexcept { return m_Value != d.m_Value; }
    constexpr bool operator>=(const Degree& d) const noexcept { return m_Value >= d.m_Value; }
    constexpr bool operator>(const Degree& d)  const noexcept { return m_Value >  d.m_Value; }

private:
    float m_Value;
};

// ═══════════════════════════════════════════════════════════════
// 跨类型内联实现（两个类都已经完整定义）
// ═══════════════════════════════════════════════════════════════

// ── Radian → Degree 方法 ──
inline float Radian::valueDegrees() const noexcept { return m_Value * Math_fRad2Deg; }

inline Radian  Radian::operator+(const Degree& d) const noexcept { return Radian(m_Value + d.valueRadians()); }
inline Radian  Radian::operator-(const Degree& d) const noexcept { return Radian(m_Value - d.valueRadians()); }
inline Radian& Radian::operator+=(const Degree& d) noexcept { m_Value += d.valueRadians(); return *this; }
inline Radian& Radian::operator-=(const Degree& d) noexcept { m_Value -= d.valueRadians(); return *this; }

// ── Degree → Radian 方法 ──
inline float Degree::valueRadians() const noexcept { return m_Value * Math_fDeg2Rad; }

// ── 自由函数: 标量 * 角度 ──
inline constexpr Radian operator*(float a, const Radian& b) noexcept { return Radian(a * b.valueRadians()); }
inline constexpr Radian operator/(float a, const Radian& b) noexcept { return Radian(a / b.valueRadians()); }
inline constexpr Degree operator*(float a, const Degree& b) noexcept { return Degree(a * b.valueDegrees()); }
inline constexpr Degree operator/(float a, const Degree& b) noexcept { return Degree(a / b.valueDegrees()); }

} // namespace Crumb
