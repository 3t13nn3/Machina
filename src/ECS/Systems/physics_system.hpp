#pragma once

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../Components/transform.hpp"

namespace ecs {
class PhysicsSystem : public System {
  public:
	void Update();
};
} // namespace ecs
