#pragma once

#include <array>
#include <cassert>
#include <queue>

#include "../Type/ecs_type.hpp"

namespace ecs {
class EntityManager {
public:
  EntityManager();

  Entity CreateEntity();

  void DestroyEntity(Entity entity);

  void SetSignature(Entity entity, Signature signature);

  Signature GetSignature(Entity entity);

private:
  std::queue<Entity> availableEntities{};
  std::array<Signature, MAX_ENTITIES> signatures{};
  uint32_t livingEntityCount{};
};
} // namespace ecs