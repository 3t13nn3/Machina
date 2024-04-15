#pragma once

#include "component_manager.hpp"

using namespace ecs;

// Define each type at compile time for the template class
#define COMPILE_FOR_TYPE(Types) Types(int) Types(double)

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
