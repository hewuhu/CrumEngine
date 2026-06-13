export module crumb.core:tool.color;
import crumb.core.math;

export namespace Crumb {
// ═══════════════════════════════════════════════════════════════
// Color — RGBA 颜色（线性空间）
// ═══════════════════════════════════════════════════════════════
struct Color
{
    float r{1.0f};
    float g{1.0f};
    float b{1.0f};
    float a{1.0f};

    constexpr Color() = default;
    constexpr Color(float r_, float g_, float b_, float a_ = 1.0f) noexcept : r(r_), g(g_), b(b_), a(a_) {}

    Vector3 toVector3() const noexcept { return Vector3(r, g, b); }
};

} // namespace Crumb