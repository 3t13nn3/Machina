#pragma once

#include <glm/glm.hpp>

namespace ecs {

struct Camera {
  glm::mat4 projectionMatrix{1.f};
  glm::mat4 viewMatrix{1.f};
  glm::mat4 inverseViewMatrix{1.f};

  const glm::vec3 getPosition() const { return glm::vec3(inverseViewMatrix[3]); }
};

} // namespace ecs