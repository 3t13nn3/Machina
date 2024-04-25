#pragma once

#include <glm/vec3.hpp>

namespace ecs {

struct RigidBody {
  glm::vec3 velocity;
  glm::vec3 acceleration;
  float mass;
};

} // namespace ecs