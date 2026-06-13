export module crumb.core.math:quaternion;

import std;
import glm;

import :angle;
import :vector;
import :matrix;


export namespace Crumb {

// ═══════════════════════════════════════════════════════════════
// Quaternion
// ═══════════════════════════════════════════════════════════════
class Quaternion {
public:
    float w{1.f}, x{0.f}, y{0.f}, z{0.f};

    Quaternion() = default;
    constexpr Quaternion(float w_, float x_, float y_, float z_) noexcept : w(w_), x(x_), y(y_), z(z_) {}

    explicit Quaternion(const glm::quat& q) noexcept : w(q.w), x(q.x), y(q.y), z(q.z) {}

    // 从旋转矩阵构造
    explicit Quaternion(const Matrix3x3& rot) noexcept { fromRotationMatrix(rot); }

    // 从角度/轴构造
    Quaternion(const Radian& angle, const Vector3& axis) noexcept { fromAngleAxis(angle, axis); }

    // 从三个正交轴构造
    Quaternion(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis) noexcept {
        fromAxes(xAxis, yAxis, zAxis);
    }

    // ── GLM 互操作 ──
    glm::quat toGlm() const noexcept { return glm::quat(w, x, y, z); }
    static Quaternion fromGlm(const glm::quat& q) noexcept { return Quaternion(q.w, q.x, q.y, q.z); }

    // ── 访问器 ──
    float*       ptr() noexcept { return &w; }
    const float* ptr() const noexcept { return &w; }

    // ── 旋转矩阵互转 ──
    void fromRotationMatrix(const Matrix3x3& rot) noexcept {
        glm::quat q = glm::quat_cast(rot.toGlm());
        w = q.w; x = q.x; y = q.y; z = q.z;
    }
    void toRotationMatrix(Matrix3x3& rot) const noexcept {
        rot = Matrix3x3(glm::mat3_cast(toGlm()));
    }
    void toRotationMatrix(Matrix4x4& rot) const noexcept {
        rot = Matrix4x4(glm::mat4_cast(toGlm()));
    }

    // ── 角度/轴 ──
    void fromAngleAxis(const Radian& angle, const Vector3& axis) noexcept {
        glm::quat q = glm::angleAxis(angle.valueRadians(), axis.toGlm());
        w = q.w; x = q.x; y = q.y; z = q.z;
    }
    static Quaternion getQuaternionFromAngleAxis(const Radian& angle, const Vector3& axis) noexcept {
        return Quaternion(glm::angleAxis(angle.valueRadians(), axis.toGlm()));
    }
    void toAngleAxis(Radian& outAngle, Vector3& outAxis) const noexcept {
        glm::quat q = toGlm();
        outAngle = Radian(glm::angle(q));
        outAxis  = Vector3::fromGlm(glm::axis(q));
    }

    // ── 方向 ──
    void fromDirection(const Vector3& direction, const Vector3& upDirection) noexcept {
        glm::quat q = glm::quatLookAt(glm::normalize(direction.toGlm()), glm::normalize(upDirection.toGlm()));
        w = q.w; x = q.x; y = q.y; z = q.z;
    }
    static Quaternion getQuaternionFromDirection(const Vector3& direction, const Vector3& upDirection) noexcept {
        return Quaternion(glm::quatLookAt(glm::normalize(direction.toGlm()), glm::normalize(upDirection.toGlm())));
    }

    // ── 轴 ──
    void fromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis) noexcept {
        Matrix3x3 rot;
        rot.fromAxes(xAxis, yAxis, zAxis);
        fromRotationMatrix(rot);
    }
    void toAxes(Vector3& outX, Vector3& outY, Vector3& outZ) const noexcept {
        Matrix3x3 rot;
        toRotationMatrix(rot);
        outX = rot.getColumn(0);
        outY = rot.getColumn(1);
        outZ = rot.getColumn(2);
    }

    Vector3 xAxis() const noexcept { return Vector3::fromGlm(toGlm() * glm::vec3(1, 0, 0)); }
    Vector3 yAxis() const noexcept { return Vector3::fromGlm(toGlm() * glm::vec3(0, 1, 0)); }
    Vector3 zAxis() const noexcept { return Vector3::fromGlm(toGlm() * glm::vec3(0, 0, 1)); }

    // ── 欧拉角 ──
    Radian getRoll(bool reprojectAxis = true) const noexcept {
        glm::vec3 euler = glm::eulerAngles(toGlm());
        (void)reprojectAxis;
        return Radian(euler.z);  // roll = z (bank)
    }
    Radian getPitch(bool reprojectAxis = true) const noexcept {
        glm::vec3 euler = glm::eulerAngles(toGlm());
        (void)reprojectAxis;
        return Radian(euler.x);  // pitch = x
    }
    Radian getYaw(bool reprojectAxis = true) const noexcept {
        glm::vec3 euler = glm::eulerAngles(toGlm());
        (void)reprojectAxis;
        return Radian(euler.y);  // yaw = y
    }

    // ── 运算 ──
    float dot(const Quaternion& other) const noexcept { return w * other.w + x * other.x + y * other.y + z * other.z; }
    float length() const noexcept { return std::sqrt(w * w + x * x + y * y + z * z); }
    float squaredLength() const noexcept { return w * w + x * x + y * y + z * z; }

    void normalise() noexcept {
        float len = length();
        if (len > 0.f) { w /= len; x /= len; y /= len; z /= len; }
    }
    Quaternion normalisedCopy() const noexcept {
        Quaternion r = *this;
        r.normalise();
        return r;
    }

    Quaternion inverse() const noexcept {
        float sqLen = w * w + x * x + y * y + z * z;
        if (sqLen > 0.f) {
            float inv = 1.0f / sqLen;
            return Quaternion(w * inv, -x * inv, -y * inv, -z * inv);
        }
        return ZERO;
    }
    Quaternion conjugate() const noexcept { return Quaternion(w, -x, -y, -z); }

    Vector3 operator*(const Vector3& v) const noexcept {
        return Vector3::fromGlm(toGlm() * v.toGlm());
    }

    // ── 插值 ──
    static Quaternion sLerp(float t, const Quaternion& a, const Quaternion& b, bool shortestPath = false) noexcept {
        (void)shortestPath;
        return Quaternion(glm::slerp(a.toGlm(), b.toGlm(), t));
    }

    static Quaternion nLerp(float t, const Quaternion& a, const Quaternion& b, bool shortestPath = false) noexcept {
        // nLerp: linear mix + normalize
        if (shortestPath && glm::dot(a.toGlm(), b.toGlm()) < 0.0f) {
            Quaternion negB(-b.w, -b.x, -b.y, -b.z);
            glm::quat mixed = glm::mix(a.toGlm(), negB.toGlm(), t);
            return Quaternion(glm::normalize(mixed));
        }
        glm::quat mixed = glm::mix(a.toGlm(), b.toGlm(), t);
        return Quaternion(glm::normalize(mixed));
    }

    // ── 运算符 ──
    Quaternion operator+(const Quaternion& rhs) const noexcept {
        return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
    }
    Quaternion operator-(const Quaternion& rhs) const noexcept {
        return Quaternion(w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z);
    }
    Quaternion operator*(const Quaternion& rhs) const noexcept {
        return Quaternion(glm::quat(toGlm() * rhs.toGlm()));
    }
    Quaternion operator*(float s) const noexcept { return Quaternion(w * s, x * s, y * s, z * s); }
    Quaternion operator/(float s) const noexcept { return Quaternion(w / s, x / s, y / s, z / s); }
    Quaternion operator-() const noexcept { return Quaternion(-w, -x, -y, -z); }

    bool operator==(const Quaternion& rhs) const noexcept { return w == rhs.w && x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!=(const Quaternion& rhs) const noexcept { return !(*this == rhs); }

    friend Quaternion operator*(float s, const Quaternion& q) noexcept { return q * s; }

    bool isNaN() const noexcept { return std::isnan(w) || std::isnan(x) || std::isnan(y) || std::isnan(z); }

    // ── 静态常量 ──
    static const Quaternion ZERO;
    static const Quaternion IDENTITY;
};

inline const Quaternion Quaternion::ZERO(0, 0, 0, 0);
inline const Quaternion Quaternion::IDENTITY(1, 0, 0, 0);

// ═══════════════════════════════════════════════════════════════
// 跨分区方法实现 — Vector3::getRotationTo
// ═══════════════════════════════════════════════════════════════

inline Quaternion Vector3::getRotationTo(const Vector3& dest, const Vector3& fallbackAxis) const {
    Vector3 v0 = this->normalisedCopy();
    Vector3 v1 = dest.normalisedCopy();

    float d = v0.dotProduct(v1);

    if (d >= 1.0f) {
        return Quaternion::IDENTITY;
    }
    if (d < (1e-6f - 1.0f)) {
        // 180 度旋转
        if (!fallbackAxis.isZero()) {
            return Quaternion(Radian(Math_PI), fallbackAxis);
        } else {
            Vector3 axis = Vector3::UNIT_X.crossProduct(*this);
            if (axis.isZeroLength()) axis = Vector3::UNIT_Y.crossProduct(*this);
            return Quaternion(Radian(Math_PI), axis.normalisedCopy());
        }
    }

    float s = std::sqrt((1.0f + d) * 2.0f);
    float invs = 1.0f / s;
    Vector3 c = v0.crossProduct(v1);

    Quaternion q;
    q.x = c.x * invs;
    q.y = c.y * invs;
    q.z = c.z * invs;
    q.w = s * 0.5f;
    q.normalise();
    return q;
}

// ═══════════════════════════════════════════════════════════════
// 跨分区方法实现 — Matrix4x4::extractQuaternion / makeTransform / decomposition
// ═══════════════════════════════════════════════════════════════

inline Quaternion Matrix4x4::extractQuaternion() const noexcept {
    Matrix3x3 m3;
    extract3x3Matrix(m3);
    return Quaternion(m3);
}

inline void Matrix4x4::makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation) noexcept {
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position.toGlm());
    glm::mat4 R = glm::mat4_cast(orientation.toGlm());
    glm::mat4 S = glm::scale(glm::mat4(1.0f), scale.toGlm());
    m_Data = T * R * S;
}

inline void Matrix4x4::makeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation) noexcept {
    glm::mat4 T_inv = glm::translate(glm::mat4(1.0f), -position.toGlm());
    glm::mat4 R_inv = glm::mat4_cast(glm::conjugate(orientation.toGlm()));
    glm::mat4 S_inv = glm::scale(glm::mat4(1.0f), 1.0f / scale.toGlm());
    m_Data = S_inv * R_inv * T_inv;
}

inline void Matrix4x4::decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const noexcept {
    position = getTrans();

    // 提取缩放（列向量长度）
    scale.x = glm::length(glm::vec3(m_Data[0]));
    scale.y = glm::length(glm::vec3(m_Data[1]));
    scale.z = glm::length(glm::vec3(m_Data[2]));

    // 移除缩放后提取旋转
    glm::mat3 rotMat(m_Data);
    if (scale.x > 0) rotMat[0] /= scale.x;
    if (scale.y > 0) rotMat[1] /= scale.y;
    if (scale.z > 0) rotMat[2] /= scale.z;
    orientation = Quaternion(glm::quat_cast(rotMat));
}

inline void Matrix4x4::decompositionWithoutScale(Vector3& position, Quaternion& rotation) const noexcept {
    position = getTrans();
    rotation = Quaternion(glm::quat_cast(glm::mat3(m_Data)));
}

} // namespace Crumb
