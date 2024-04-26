#pragma once

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../Components/gravity.hpp"
#include "../Components/rigid_body.hpp"
#include "../Components/transform.hpp"

#include "../../vulkan/frame_info.hpp"

#include <memory>

using namespace vu;

namespace ecs {
class CollisionSystem : public System {
public:
  CollisionSystem();

  void update(FrameInfo &frameInfo);
};
} // namespace ecs
