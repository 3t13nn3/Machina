#pragma once

#include <glm/vec3.hpp>

namespace ecs {

const float GRAVITY_CONSTANT = 8.91 / 60; // 60 IS FPS

struct Gravity {
  glm::vec3 force;
};

} // namespace ecs