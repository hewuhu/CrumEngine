module;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

export module glm;

export namespace glm {
using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
using glm::uvec2;
using glm::uvec3;
using glm::uvec4;

using glm::mat2;
using glm::mat3;
using glm::mat4;

using glm::quat;

// gtc/matrix_transform
using glm::translate;
using glm::rotate;
using glm::scale;
using glm::ortho;
using glm::perspective;
using glm::lookAt;

using glm::radians;
using glm::degrees;

using glm::normalize;
using glm::dot;
using glm::cross;
using glm::length;
using glm::distance;
using glm::reflect;

using glm::inverse;
using glm::transpose;
using glm::determinant;

// gtc/type_ptr
using glm::value_ptr;
using glm::make_mat3;
using glm::make_mat4;

// gtc/quaternion
using glm::quat_cast;
using glm::mat3_cast;
using glm::mat4_cast;
using glm::angleAxis;
using glm::angle;
using glm::axis;
using glm::quatLookAt;
using glm::eulerAngles;
using glm::slerp;
using glm::mix;
using glm::conjugate;

// ── 运算符 ──
using glm::operator==;
using glm::operator!=;
using glm::operator+;
using glm::operator-;
using glm::operator*;
using glm::operator/;
} // namespace glm
