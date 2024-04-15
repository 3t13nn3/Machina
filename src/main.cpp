#include "component_array.hpp"
#include "component_manager.hpp"
#include "entity_manager.hpp"

#include <iostream>

int main() {
  ecs::EntityManager em{};
  ecs::ComponentArray<int> ca{};
  ecs::ComponentManager cm{};

  for (ecs::Entity e{0}; e < ecs::MAX_ENTITIES; ++e) {
    em.CreateEntity();
    em.SetSignature(e, ecs::Signature(e));
    // std::cout << em.GetSignature(e) << std::endl;
    ca.InsertData(e, int(e));
    ca.RemoveData(e);
    // std::cout << ca.GetData(e) << std::endl;
  }

  for (ecs::Entity e{0}; e < ecs::MAX_ENTITIES; ++e) {
    em.DeleteEntity(e);
    ca.EntityDestroyed(e);
  }

  cm.RegisterComponent<double>();
  std::cout << double(cm.GetComponentType<double>()) << std::endl;
  cm.AddComponent<double>(ecs::Entity{42}, 99999.9);
  std::cout << cm.GetComponent<double>(ecs::Entity{42}) << std::endl;
  cm.RemoveComponent<double>(ecs::Entity{42});
  cm.EntityDestroyed(ecs::Entity{42});
  return 0;
}
