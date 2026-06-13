export module crumb.core.math:matrix;

import std;
import glm;

import :vector;
import :angle;

export namespace Crumb {

class Quaternion;

// ═══════════════════════════════════════════════════════════════
// Matrix3x3 — 3x3 列主序矩阵
// ═══════════════════════════════════════════════════════════════
class Matrix3x3 {
public:
    Matrix3x3() noexcept : m_Data(1.0f) {}  // 单位矩阵

    explicit Matrix3x3(const glm::mat3& m) noexcept : m_Data(m) {}

    Matrix3x3(float m00, float m01, float m02,
              float m10, float m11, float m12,
              float m20, float m21, float m22) noexcept
        : m_Data(m00, m01, m02, m10, m11, m12, m20, m21, m22) {}

    explicit Matrix3x3(const float float_array[9]) noexcept
        : m_Data(glm::make_mat3(float_array)) {}

    Matrix3x3(const Vector3& row0, const Vector3& row1, const Vector3& row2) noexcept
        : m_Data(row0.x, row0.y, row0.z,
                 row1.x, row1.y, row1.z,
                 row2.x, row2.y, row2.z) {}

    // ── GLM 互操作 ──
    const glm::mat3& toGlm() const noexcept { return m_Data; }
    glm::mat3&       toGlm() noexcept { return m_Data; }
    static Matrix3x3 fromGlm(const glm::mat3& m) noexcept { return Matrix3x3(m); }

    // ── 数据转换 ──
    void fromData(const float float_array[9]) noexcept { m_Data = glm::make_mat3(float_array); }
    void toData(float float_array[9]) const noexcept {
        const float* src = glm::value_ptr(m_Data);
        for (int i = 0; i < 9; ++i) float_array[i] = src[i];
    }

    // ── 元素访问: operator()(row, col) ──
    float  operator()(std::size_t row, std::size_t col) const noexcept { return m_Data[col][row]; }
    float& operator()(std::size_t row, std::size_t col) noexcept { return m_Data[col][row]; }

    // ── 列访问 ──
    Vector3 getColumn(std::size_t col) const noexcept {
        const auto& c = m_Data[col];
        return Vector3(c.x, c.y, c.z);
    }
    void setColumn(std::size_t col, const Vector3& v) noexcept {
        m_Data[col] = glm::vec3(v.x, v.y, v.z);
    }

    // ── 轴构造 ──
    void fromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis) noexcept {
        setColumn(0, xAxis);
        setColumn(1, yAxis);
        setColumn(2, zAxis);
    }

    // ── 运算 ──
    Matrix3x3 transpose() const noexcept { return Matrix3x3(glm::transpose(m_Data)); }

    bool inverse(Matrix3x3& outInv, float tolerance = 1e-06f) const noexcept {
        float det = determinant();
        if (std::fabs(det) <= tolerance) return false;
        outInv.m_Data = glm::inverse(m_Data);
        return true;
    }

    Matrix3x3 inverse(float tolerance = 1e-06f) const noexcept {
        Matrix3x3 result;
        inverse(result, tolerance);
        return result;
    }

    float determinant() const noexcept { return glm::determinant(m_Data); }

    // ── 缩放 ──
    static Matrix3x3 scale(const Vector3& s) noexcept {
        return Matrix3x3(s.x, 0, 0,
                         0, s.y, 0,
                         0, 0, s.z);
    }

    // ── 运算符 ──
    Matrix3x3 operator+(const Matrix3x3& rhs) const noexcept { return Matrix3x3(m_Data + rhs.m_Data); }
    Matrix3x3 operator-(const Matrix3x3& rhs) const noexcept { return Matrix3x3(m_Data - rhs.m_Data); }
    Matrix3x3 operator*(const Matrix3x3& rhs) const noexcept { return Matrix3x3(m_Data * rhs.m_Data); }
    Vector3   operator*(const Vector3& v) const noexcept { return Vector3::fromGlm(m_Data * v.toGlm()); }
    Matrix3x3 operator*(float s) const noexcept { return Matrix3x3(m_Data * s); }
    Matrix3x3 operator-() const noexcept { return Matrix3x3(-m_Data); }

    bool operator==(const Matrix3x3& rhs) const noexcept { return m_Data == rhs.m_Data; }
    bool operator!=(const Matrix3x3& rhs) const noexcept { return m_Data != rhs.m_Data; }

    friend Vector3   operator*(const Vector3& v, const Matrix3x3& m) noexcept { return Vector3::fromGlm(v.toGlm() * m.m_Data); }
    friend Matrix3x3 operator*(float s, const Matrix3x3& m) noexcept { return Matrix3x3(s * m.m_Data); }

    // ── 静态常量 ──
    static const Matrix3x3 ZERO;
    static const Matrix3x3 IDENTITY;

private:
    glm::mat3 m_Data;
};

inline const Matrix3x3 Matrix3x3::ZERO(glm::mat3(0.0f));
inline const Matrix3x3 Matrix3x3::IDENTITY(glm::mat3(1.0f));

// ═══════════════════════════════════════════════════════════════
// Matrix4x4 — 4x4 列主序矩阵
// ═══════════════════════════════════════════════════════════════
class Matrix4x4 {
public:
    Matrix4x4() noexcept : m_Data(1.0f) {}

    explicit Matrix4x4(const glm::mat4& m) noexcept : m_Data(m) {}

    Matrix4x4(float m00, float m01, float m02, float m03,
              float m10, float m11, float m12, float m13,
              float m20, float m21, float m22, float m23,
              float m30, float m31, float m32, float m33) noexcept
        : m_Data(m00, m01, m02, m03,
                 m10, m11, m12, m13,
                 m20, m21, m22, m23,
                 m30, m31, m32, m33) {}

    explicit Matrix4x4(const float float_array[16]) noexcept
        : m_Data(glm::make_mat4(float_array)) {}

    Matrix4x4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3) noexcept
        : m_Data(row0.x, row0.y, row0.z, row0.w,
                 row1.x, row1.y, row1.z, row1.w,
                 row2.x, row2.y, row2.z, row2.w,
                 row3.x, row3.y, row3.z, row3.w) {}

    // ── GLM 互操作 ──
    const glm::mat4& toGlm() const noexcept { return m_Data; }
    glm::mat4&       toGlm() noexcept { return m_Data; }
    static Matrix4x4 fromGlm(const glm::mat4& m) noexcept { return Matrix4x4(m); }

    // ── 数据转换 ──
    void fromData(const float float_array[16]) noexcept { m_Data = glm::make_mat4(float_array); }
    void toData(float float_array[16]) const noexcept {
        const float* src = glm::value_ptr(m_Data);
        for (int i = 0; i < 16; ++i) float_array[i] = src[i];
    }

    // ── 元素访问: operator()(row, col) ──
    float  operator()(std::size_t row, std::size_t col) const noexcept { return m_Data[col][row]; }
    float& operator()(std::size_t row, std::size_t col) noexcept { return m_Data[col][row]; }

    // ── 平直访问（value_ptr 顺序） ──
    float  operator[](std::size_t i) const noexcept { return glm::value_ptr(m_Data)[i]; }
    float& operator[](std::size_t i) noexcept { return glm::value_ptr(m_Data)[i]; }

    // ── 基本矩阵运算 ──
    Matrix4x4 concatenate(const Matrix4x4& m2) const noexcept { return Matrix4x4(m_Data * m2.m_Data); }
    Matrix4x4 transpose() const noexcept { return Matrix4x4(glm::transpose(m_Data)); }
    Matrix4x4 inverse() const noexcept { return Matrix4x4(glm::inverse(m_Data)); }
    float     determinant() const noexcept { return glm::determinant(m_Data); }

    // ── 子式 ──
    float getMinor(std::size_t r0, std::size_t r1, std::size_t r2, std::size_t c0, std::size_t c1, std::size_t c2) const noexcept {
        return m_Data[c0][r0] * (m_Data[c1][r1] * m_Data[c2][r2] - m_Data[c2][r1] * m_Data[c1][r2]) -
               m_Data[c1][r0] * (m_Data[c0][r1] * m_Data[c2][r2] - m_Data[c2][r1] * m_Data[c0][r2]) +
               m_Data[c2][r0] * (m_Data[c0][r1] * m_Data[c1][r2] - m_Data[c1][r1] * m_Data[c0][r2]);
    }

    // ── 平移 ──
    void setTrans(const Vector3& v) noexcept {
        m_Data[3] = glm::vec4(v.x, v.y, v.z, m_Data[3].w);
    }
    Vector3 getTrans() const noexcept {
        return Vector3(m_Data[3].x, m_Data[3].y, m_Data[3].z);
    }

    void makeTrans(const Vector3& v) noexcept { *this = getTrans(v); }
    void makeTrans(float tx, float ty, float tz) noexcept { *this = getTrans(tx, ty, tz); }

    static Matrix4x4 getTrans(const Vector3& v) noexcept {
        return Matrix4x4(glm::translate(glm::mat4(1.0f), v.toGlm()));
    }
    static Matrix4x4 getTrans(float tx, float ty, float tz) noexcept {
        return Matrix4x4(glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz)));
    }

    // ── 缩放 ──
    void setScale(const Vector3& v) noexcept {
        m_Data[0][0] = v.x;
        m_Data[1][1] = v.y;
        m_Data[2][2] = v.z;
    }
    static Matrix4x4 getScale(const Vector3& v) noexcept {
        return Matrix4x4(glm::scale(glm::mat4(1.0f), v.toGlm()));
    }
    static Matrix4x4 buildScaleMatrix(float sx, float sy, float sz) noexcept {
        return Matrix4x4(glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz)));
    }

    // ── 3x3 提取/设置 ──
    void setMatrix3x3(const Matrix3x3& mat3) noexcept {
        for (int col = 0; col < 3; ++col)
            for (int row = 0; row < 3; ++row)
                m_Data[col][row] = mat3(row, col);
        m_Data[3] = glm::vec4(0, 0, 0, 1);
    }
    void extract3x3Matrix(Matrix3x3& m3x3) const noexcept {
        m3x3 = Matrix3x3(glm::mat3(m_Data));
    }
    void extractAxes(Vector3& outX, Vector3& outY, Vector3& outZ) const noexcept {
        outX = Vector3(m_Data[0].x, m_Data[0].y, m_Data[0].z); outX.normalise();
        outY = Vector3(m_Data[1].x, m_Data[1].y, m_Data[1].z); outY.normalise();
        outZ = Vector3(m_Data[2].x, m_Data[2].y, m_Data[2].z); outZ.normalise();
    }

    // ── 缩放检测 ──
    bool hasScale() const noexcept {
        auto checkCol = [](const glm::vec4& c) {
            float t = c.x * c.x + c.y * c.y + c.z * c.z;
            return std::fabs(t - 1.0f) > 1e-04f;
        };
        return checkCol(m_Data[0]) || checkCol(m_Data[1]) || checkCol(m_Data[2]);
    }
    bool hasNegativeScale() const noexcept { return determinant() < 0.0f; }

    // ── 提取四元数（体在 quaternion 分区定义） ──
    Quaternion extractQuaternion() const noexcept;

    // ── 仿射检查 ──
    bool isAffine() const noexcept {
        return m_Data[3][0] == 0 && m_Data[3][1] == 0 && m_Data[3][2] == 0 && m_Data[3][3] == 1;
    }

    Matrix4x4 inverseAffine() const noexcept { return Matrix4x4(glm::inverse(m_Data)); }

    Matrix4x4 concatenateAffine(const Matrix4x4& m2) const noexcept {
        // 优化的仿射乘法：跳过底行计算
        glm::mat4 r;
        r[0] = m_Data * m2.m_Data[0];
        r[1] = m_Data * m2.m_Data[1];
        r[2] = m_Data * m2.m_Data[2];
        r[3] = m_Data * m2.m_Data[3];
        return Matrix4x4(r);
    }

    Vector3 transformAffine(const Vector3& v) const noexcept {
        return Vector3::fromGlm(glm::vec3(m_Data * glm::vec4(v.toGlm(), 1.0f)));
    }
    Vector4 transformAffine(const Vector4& v) const noexcept {
        return Vector4::fromGlm(m_Data * v.toGlm());
    }

    Vector3 transformCoord(const Vector3& v) const noexcept {
        glm::vec4 temp = m_Data * glm::vec4(v.toGlm(), 1.0f);
        if (temp.w == 0.0f) return Vector3::ZERO;
        return Vector3::fromGlm(glm::vec3(temp) / temp.w);
    }

    // ── 变换组合 ──
    void makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation) noexcept;
    void makeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation) noexcept;
    void decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const noexcept;
    void decompositionWithoutScale(Vector3& position, Quaternion& rotation) const noexcept;

    // ── 投影矩阵 ──
    static Matrix4x4 createOrtho(float left, float right, float bottom, float top,
                                  float zNear = -1.0f, float zFar = 1.0f) noexcept {
        return Matrix4x4(glm::ortho(left, right, bottom, top, zNear, zFar));
    }

    static Matrix4x4 createPerspective(float fovY, float aspect,
                                        float zNear, float zFar) noexcept {
        return Matrix4x4(glm::perspective(fovY, aspect, zNear, zFar));
    }

    // ── 特殊矩阵 ──
    static Matrix4x4 buildViewportMatrix(std::uint32_t width, std::uint32_t height) noexcept {
        return Matrix4x4(0.5f * (float)width, 0.0f, 0.0f, 0.5f * (float)width,
                         0.0f, -0.5f * (float)height, 0.0f, 0.5f * (float)height,
                         0.0f, 0.0f, -1.0f, 1.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);
    }

    static Matrix4x4 mirrorMatrix(const Vector4& mirrorPlane) noexcept {
        const float x = mirrorPlane.x, y = mirrorPlane.y, z = mirrorPlane.z, w = mirrorPlane.w;
        return Matrix4x4(-2*x*x + 1,   -2*y*x,       -2*z*x,       -2*w*x,
                         -2*x*y,       -2*y*y + 1,   -2*z*y,       -2*w*y,
                         -2*x*z,       -2*y*z,       -2*z*z + 1,   -2*w*z,
                         0,             0,             0,             1);
    }

    static Matrix4x4 rotationMatrix(const Vector3& normal) noexcept {
        Vector3 up(0, 0, 1);
        if (std::fabs(normal.z) > 0.999f) up = Vector3(0, 1, 0);
        Vector3 left = up.crossProduct(normal);
        up = normal.crossProduct(left);
        left.normalise();
        up.normalise();
        Matrix4x4 result;
        result.setMatrix3x3(Matrix3x3(left, up, normal));
        return result.transpose();
    }

    // ── 运算符 ──
    Matrix4x4 operator*(const Matrix4x4& m2) const noexcept { return concatenate(m2); }
    Vector3   operator*(const Vector3& v) const noexcept {
        glm::vec4 tmp = m_Data * glm::vec4(v.toGlm(), 1.0f);
        float invW = 1.0f / tmp.w;
        return Vector3(tmp.x * invW, tmp.y * invW, tmp.z * invW);
    }
    Vector4   operator*(const Vector4& v) const noexcept { return Vector4::fromGlm(m_Data * v.toGlm()); }
    Matrix4x4 operator*(float s) const noexcept { return Matrix4x4(m_Data * s); }
    Matrix4x4 operator+(const Matrix4x4& m2) const noexcept { return Matrix4x4(m_Data + m2.m_Data); }
    Matrix4x4 operator-(const Matrix4x4& m2) const noexcept { return Matrix4x4(m_Data - m2.m_Data); }

    bool operator==(const Matrix4x4& m2) const noexcept { return m_Data == m2.m_Data; }
    bool operator!=(const Matrix4x4& m2) const noexcept { return m_Data != m2.m_Data; }

    friend Vector4   operator*(const Vector4& v, const Matrix4x4& m) noexcept { return Vector4::fromGlm(v.toGlm() * m.m_Data); }
    friend Matrix4x4 operator*(float s, const Matrix4x4& m) noexcept { return Matrix4x4(s * m.m_Data); }

    // ── 静态常量 ──
    static const Matrix4x4 ZERO;
    static const Matrix4x4 ZEROAFFINE;
    static const Matrix4x4 IDENTITY;

private:
    glm::mat4 m_Data;
};

inline const Matrix4x4 Matrix4x4::ZERO(glm::mat4(0.0f));
inline const Matrix4x4 Matrix4x4::ZEROAFFINE(glm::mat4(0.0f, 0.0f, 0.0f, 0.0f,
                                                        0.0f, 0.0f, 0.0f, 0.0f,
                                                        0.0f, 0.0f, 0.0f, 0.0f,
                                                        0.0f, 0.0f, 0.0f, 1.0f));
inline const Matrix4x4 Matrix4x4::IDENTITY(glm::mat4(1.0f));

} // namespace Crumb
