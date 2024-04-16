#pragma once

#include "component_manager.hpp"
#include "entity_manager.hpp"
#include "system_manager.hpp"

#include "../Type/ecs_type.hpp"

#include <memory>

namespace ecs {
class Centralizer {
public:
  Centralizer();

  Entity CreateEntity();

  void DestroyEntity(Entity entity);

  template <typename T> void RegisterComponent();

  template <typename T> void AddComponent(Entity entity, T component);

  template <typename T> void RemoveComponent(Entity entity);

  template <typename T> T &GetComponent(Entity entity);

  template <typename T> ComponentType GetComponentType();

  template <typename T> std::shared_ptr<T> RegisterSystem();

  template <typename T> void SetSystemSignature(Signature signature);

private:
  std::unique_ptr<ComponentManager> componentManager;
  std::unique_ptr<EntityManager> entityManager;
  std::unique_ptr<SystemManager> systemManager;
};
} // namespace ecs