#include "entity_manager.hpp"

namespace ecs {
EntityManager::EntityManager() {
  for (Entity entity{0}; entity < MAX_ENTITIES; ++entity) {
    availableEntities.push(entity);
  }
}

Entity EntityManager::CreateEntity() {
  // check if we can add an entity
  assert(livingEntityCount < MAX_ENTITIES &&
         "CreateEntity : Max entities count is reached.");

  // Recover the entity and remove it from the availale queue
  Entity curr = availableEntities.front();
  availableEntities.pop();
  ++livingEntityCount;

  return curr;
}

void EntityManager::DeleteEntity(Entity entity) {
  // check if the current entity exist
  assert(entity < MAX_ENTITIES &&
         "DeleteEntity : Entity may be already destroyed or out of range.");

  // set all its bit field to 0
  signatures[entity].reset();

  // push our entity at the end of the queue
  availableEntities.push(entity);
  --livingEntityCount;
}

void EntityManager::SetSignature(Entity entity, Signature signature) {
  // check if the entity is in range
  assert(entity < MAX_ENTITIES && "SetSignatue : Entity may be out of range.");

  // assign signature to the entity
  signatures[entity] = signature;
}

Signature EntityManager::GetSignature(Entity entity) {
  assert(entity < MAX_ENTITIES && "GetSignatue : Entity may be out of range.");
  return signatures[entity];
}

} // namespace ecs