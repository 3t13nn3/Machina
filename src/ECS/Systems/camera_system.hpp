#pragma once

#include "../../vulkan/uniform_buffer_type.hpp"

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../Components/camera.hpp"
#include "../Components/transform.hpp"

#include "../../vulkan/frame_info.hpp"

using namespace vu;

namespace ecs {
class CameraSystem : public System {

public:
  void lookAt(Entity cameraEntity, const glm::vec3 &direction);

  void update(GlobalUbo &ubo, float aspect, Entity e);

  void setPerspectiveProjection(float fovy, float aspect, float near, float far, Entity e);

private:
  void setViewYXZ(Entity e);
};
} // namespace ecs
