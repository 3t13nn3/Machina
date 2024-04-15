#include "component_array.hpp"

#include "template_type_macro.hpp"

COMPILE_FOR_TYPE(INSTANTIATE_COMPONENT_ARRAY_FOR_TYPE)

namespace ecs {

template <typename T>
void ComponentArray<T>::InsertData(Entity entity, T component) {
  assert(entityToIndex.find(entity) == entityToIndex.end() &&
         "InsertData : Entity is already in the component array.");

  // keep the index to data position
  entityToIndex[entity] = currentSize;
  indexToEntity[currentSize] = entity;

  // add the component to the right place
  componentArray[currentSize] = component;

  ++currentSize;
}

template <typename T> void ComponentArray<T>::RemoveData(Entity entity) {
  assert(entityToIndex.find(entity) != entityToIndex.end() &&
         "RemoveData : Entity isn't in the component array.");

  // put the last element of the array at the "to remove" entity index
  size_t removedEntityIndex = entityToIndex[entity];
  size_t LastEntityIndex = currentSize - 1;
  componentArray[removedEntityIndex] = componentArray[LastEntityIndex];

  // update map to keep the 1-1 relation between the new index and entity
  Entity lastElement = componentArray[removedEntityIndex];
  entityToIndex[lastElement] = removedEntityIndex;
  indexToEntity[removedEntityIndex] = lastElement;

  --currentSize;
}

template <typename T> T &ComponentArray<T>::GetData(Entity entity) {
  assert(entityToIndex.find(entity) != entityToIndex.end() &&
         "GetData : Entity isn't in the component array.");

  return componentArray[entityToIndex[entity]];
}

template <typename T> void ComponentArray<T>::EntityDestroyed(Entity entity) {
  RemoveData(entity);
}

} // namespace ecs