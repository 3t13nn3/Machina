#pragma once

#include "../Base/component_manager.hpp"
#include "../Systems/physics_system.hpp"

#include "../Components/transform.hpp"

using namespace ecs;

// Define each type at compile time for the template class
#define COMPILE_FOR_COMPONENT(Types) Types(Transform)

#define INSTANTIATE_COMPONENT_ARRAY_FOR_TYPE(Type)                             \
  template class ComponentArray<Type>;

#define INSTANTIATE_COMPONENT_MANAGER_FOR_TYPE(Type)                           \
  template void ComponentManager::RegisterComponent<Type>();                   \
  template ComponentType ComponentManager::GetComponentType<Type>();           \
  template void ComponentManager::AddComponent<Type>(Entity entity,            \
                                                     Type component);          \
  template void ComponentManager::RemoveComponent<Type>(Entity entity);        \
  template Type &ComponentManager::GetComponent<Type>(Entity entity);          \
  template std::shared_ptr<ComponentArray<Type>>                               \
  ComponentManager::GetComponentArray<Type>();

#define COMPILE_FOR_SYSTEM(Types) Types(PhysicsSystem)

#define INSTANTIATE_SYSTEM_MANAGER_FOR_TYPE(Type)                              \
  template std::shared_ptr<Type> SystemManager::RegisterSystem<Type>();        \
  template void SystemManager::SetSignature<Type>(Signature signature);

// Macro pour instancier les membres de la classe Centralizer pour un type donné
#define INSTANTIATE_CENTRALIZER_FOR_TYPE(Type)                                 \
  template void Centralizer::RegisterComponent<Type>();                        \
  template void Centralizer::AddComponent<Type>(Entity entity,                 \
                                                Type component);               \
  template void Centralizer::RemoveComponent<Type>(Entity entity);             \
  template Type &Centralizer::GetComponent<Type>(Entity entity);               \
  template ComponentType Centralizer::GetComponentType<Type>();

#define INSTANTIATE_CENTRALIZER_FOR_SYSTEM(Type)                               \
  template std::shared_ptr<Type> Centralizer::RegisterSystem<Type>();          \
  template void Centralizer::SetSystemSignature<Type>(Signature signature);
