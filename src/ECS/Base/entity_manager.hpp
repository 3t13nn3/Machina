#pragma once

#include <array>
#include <cassert>
#include <queue>

#include "../Type/ecs_type.hpp"

namespace ecs {
class EntityManager {
  public:
	EntityManager() {
		for (Entity entity{0}; entity < MAX_ENTITIES; ++entity) {
			availableEntities.push(entity);
		}
	}

	Entity CreateEntity() {
		// check if we can add an entity
		assert(livingEntityCount < MAX_ENTITIES &&
			   "CreateEntity : Max entities count is reached.");

		// Recover the entity and remove it from the availale queue
		Entity curr = availableEntities.front();
		availableEntities.pop();
		++livingEntityCount;

		return curr;
	}

	void DestroyEntity(Entity entity) {
		// check if the current entity exist
		assert(
			entity < MAX_ENTITIES &&
			"DestroyEntity : Entity may be already destroyed or out of range.");

		// set all its bit field to 0
		signatures[entity].reset();

		// push our entity at the end of the queue
		availableEntities.push(entity);
		--livingEntityCount;
	}

	void SetSignature(Entity entity, Signature signature) {
		// check if the entity is in range
		assert(entity < MAX_ENTITIES &&
			   "SetSignatue : Entity may be out of range.");

		// assign signature to the entity
		signatures[entity] = signature;
	}

	Signature GetSignature(Entity entity) {
		assert(entity < MAX_ENTITIES &&
			   "GetSignatue : Entity may be out of range.");
		return signatures[entity];
	}

  private:
	std::queue<Entity> availableEntities{};
	std::array<Signature, MAX_ENTITIES> signatures{};
	uint32_t livingEntityCount{};
};
} // namespace ecs