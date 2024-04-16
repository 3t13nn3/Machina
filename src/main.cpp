#include "ECS/ECS.hpp"

#include <iostream>

int main() {

  ecs::Centralizer c{};

  c.RegisterComponent<int>();
  c.RegisterComponent<double>();

  std::shared_ptr<ecs::PhysicsSystem> sys =
      c.RegisterSystem<ecs::PhysicsSystem>();

  ecs::Signature sign;
  sign.set(c.GetComponentType<int>());
  sign.set(c.GetComponentType<double>());

  c.SetSystemSignature<ecs::PhysicsSystem>(sign);

  std::vector<ecs::Entity> entities(ecs::MAX_ENTITIES);
  for (auto &e : entities) {
    e = c.CreateEntity();

    c.AddComponent(e, int{});
    c.AddComponent(e, double{});
  }

  /*
  graphicloop() {
    sys->Update();
  }
  */

  std::cout << "Machina !" << std::endl;

  return 0;
}
