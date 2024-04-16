#include "system_manager.hpp"

#include "../Type/template_type_macro.hpp"

COMPILE_FOR_SYSTEM(INSTANTIATE_SYSTEM_MANAGER_FOR_TYPE)

namespace ecs {

template <typename T> std::shared_ptr<T> SystemManager::RegisterSystem() {
  const char *typeName = typeid(T).name();
  assert(systems.find(typeName) == systems.end() &&
         "RegisterSystem : System already exist.");

  auto sys = std::make_shared<T>();
  systems.insert({typeName, sys});

  return sys;
}

template <typename T> void SystemManager::SetSignature(Signature signature) {
  const char *typeName = typeid(T).name();

  assert(systems.find(typeName) != systems.end() &&
         "SetSignature : System doesn't exist.");

  // if we want to update the signature, maybe use [] operator
  signatures.insert({typeName, signature});
}

void SystemManager::EntityDestroyed(Entity entity) {
  // Erase the entity for each systems
  for (const auto &e : systems) {
    (e.second)->entities.erase(entity);
  }
}

void SystemManager::EntitySignatureChanged(Entity entity,
                                           Signature entitySignature) {
  // browse every system
  for (const auto &e : systems) {
    auto const &type = e.first;
    auto const &system = e.second;
    auto const &systemSignature = signatures[type];

    // Check if the entity got the bit for the current system
    if ((entitySignature & systemSignature) == systemSignature) {
      system->entities.insert(entity);
    } else { // CHECK
      system->entities.erase(entity);
    }
  }
}

} // namespace ecs