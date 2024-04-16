#pragma once

#include "../Type/ecs_type.hpp"

#include <set>

namespace ecs {

class System {
public:
  std::set<Entity> entities;
};

} // namespace ecs