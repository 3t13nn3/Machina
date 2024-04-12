#include "entity_manager.hpp"
#include <iostream>

int main() {
  ecs::EntityManager em{};

  for (ecs::Entity e{0}; e < ecs::MAX_ENTITIES; ++e) {
    em.CreateEntity();
    em.SetSignature(e, ecs::Signature(e));
    std::cout << em.GetSignature(e) << std::endl;
  }

  for (ecs::Entity e{0}; e < ecs::MAX_ENTITIES; ++e) {
    em.DeleteEntity(e);
  }

  return 0;
}
