#pragma once

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../Components/camera.hpp"
#include "../Components/transform.hpp"

#include "../../vulkan/frame_info.hpp"

using namespace vu;

namespace ecs {
class CameraSystem : public System {

  public:
	void update(GlobalUbo &ubo, float aspect);

	void setPerspectiveProjection(float fovy, float aspect, float near,
								  float far);

  private:
	void setViewYXZ();
};
} // namespace ecs
