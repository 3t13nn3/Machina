#include "component_array.hpp"
#include "entity_manager.hpp"
#include <iostream>

int main() {
  ecs::EntityManager em{};

  for (ecs::Entity e{0}; e < ecs::MAX_ENTITIES; ++e) {
    em.CreateEntity();
    em.SetSignature(e, ecs::Signature(e));
    // std::cout << em.GetSignature(e) << std::endl;
  }

  for (ecs::Entity e{0}; e < ecs::MAX_ENTITIES; ++e) {
    em.DeleteEntity(e);
  }

  ecs::ComponentArray<int> ca{};
  for (ecs::Entity e{0}; e < ecs::MAX_ENTITIES; ++e) {
    ca.InsertData(e, int(e));
    ca.RemoveData(e);
    std::cout << ca.GetData(e) << std::endl;
  }

  for (ecs::Entity e{0}; e < ecs::MAX_ENTITIES; ++e) {
    ca.EntityDestroyed(e);
  }

  return 0;
}
