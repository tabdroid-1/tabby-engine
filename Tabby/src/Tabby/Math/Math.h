#pragma once

#include <glm/glm.hpp>

namespace Tabby::Math {

const float PI = 3.14159265358979323846f;
const float DEG2RAD = (PI / 180.0f);
const float RAD2DEG = (180.0f / PI);
const float EPSILON = 0.000001f;

bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

}
