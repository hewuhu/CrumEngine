export module crumb.core.math:transform;

import std;
import glm;

import :vector;
import :quaternion;
import :matrix;

export namespace Crumb {

// ═══════════════════════════════════════════════════════════════
// Transform — 位置 + 旋转 + 缩放
// ═══════════════════════════════════════════════════════════════
class Transform {
public:
    Vector3    m_Position{0, 0, 0};
    Quaternion m_Rotation{1, 0, 0, 0};
    Vector3    m_Scale{1, 1, 1};

    Transform() = default;

    Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale) noexcept
        : m_Position(position), m_Rotation(rotation), m_Scale(scale) {}

    // ── 获取 TRS 矩阵 ──
    Matrix4x4 getMatrix() const noexcept {
        Matrix4x4 result;
        result.makeTransform(m_Position, m_Scale, m_Rotation);
        return result;
    }

    // ── 访问器 ──
    const Vector3&    getPosition() const noexcept { return m_Position; }
    const Quaternion& getRotation() const noexcept { return m_Rotation; }
    const Vector3&    getScale() const noexcept { return m_Scale; }

    void setPosition(const Vector3& v) noexcept { m_Position = v; }
    void setRotation(const Quaternion& q) noexcept { m_Rotation = q; }
    void setScale(const Vector3& v) noexcept { m_Scale = v; }
};

// ═══════════════════════════════════════════════════════════════
// AxisAlignedBox — 轴对齐包围盒
// ═══════════════════════════════════════════════════════════════
class AxisAlignedBox {
public:
    AxisAlignedBox() noexcept = default;

    AxisAlignedBox(const Vector3& center, const Vector3& halfExtent) noexcept
        : m_Center(center), m_HalfExtent(halfExtent) {}

    // ── 访问器 ──
    const Vector3& getCenter() const noexcept { return m_Center; }
    const Vector3& getHalfExtent() const noexcept { return m_HalfExtent; }

    Vector3 getMinCorner() const noexcept { return m_Center - m_HalfExtent; }
    Vector3 getMaxCorner() const noexcept { return m_Center + m_HalfExtent; }

    void setCenter(const Vector3& center) noexcept { m_Center = center; }
    void setHalfExtent(const Vector3& halfExtent) noexcept { m_HalfExtent = halfExtent; }

    void update(const Vector3& center, const Vector3& halfExtent) noexcept {
        m_Center = center;
        m_HalfExtent = halfExtent;
    }

    // ── 合并点 ──
    void merge(const Vector3& point) noexcept {
        Vector3 minCorner = getMinCorner();
        Vector3 maxCorner = getMaxCorner();

        if (point.x < minCorner.x) minCorner.x = point.x;
        if (point.y < minCorner.y) minCorner.y = point.y;
        if (point.z < minCorner.z) minCorner.z = point.z;
        if (point.x > maxCorner.x) maxCorner.x = point.x;
        if (point.y > maxCorner.y) maxCorner.y = point.y;
        if (point.z > maxCorner.z) maxCorner.z = point.z;

        m_Center     = (maxCorner + minCorner) * 0.5f;
        m_HalfExtent = (maxCorner - minCorner) * 0.5f;
    }

    void merge(const AxisAlignedBox& other) noexcept {
        merge(other.getMinCorner());
        merge(other.getMaxCorner());
    }

    // ── 检测 ──
    bool contains(const Vector3& point) const noexcept {
        Vector3 minC = getMinCorner();
        Vector3 maxC = getMaxCorner();
        return point.x >= minC.x && point.x <= maxC.x &&
               point.y >= minC.y && point.y <= maxC.y &&
               point.z >= minC.z && point.z <= maxC.z;
    }

    bool intersects(const AxisAlignedBox& other) const noexcept {
        Vector3 aMin = getMinCorner(), aMax = getMaxCorner();
        Vector3 bMin = other.getMinCorner(), bMax = other.getMaxCorner();
        return aMin.x <= bMax.x && aMax.x >= bMin.x &&
               aMin.y <= bMax.y && aMax.y >= bMin.y &&
               aMin.z <= bMax.z && aMax.z >= bMin.z;
    }

    bool isEmpty() const noexcept {
        return m_HalfExtent.x <= 0.f && m_HalfExtent.y <= 0.f && m_HalfExtent.z <= 0.f;
    }

private:
    Vector3 m_Center{0, 0, 0};
    Vector3 m_HalfExtent{0, 0, 0};
};

} // namespace Crumb
