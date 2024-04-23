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
			mAvailableEntities.push(entity);
		}
	}

	Entity createEntity() {
		// check if we can add an entity
		assert(mLivingEntityCount < MAX_ENTITIES &&
			   "createEntity : Max entities count is reached.");

		// Recover the entity and remove it from the availale queue
		Entity curr = mAvailableEntities.front();
		mAvailableEntities.pop();
		++mLivingEntityCount;

		return curr;
	}

	void destroyEntity(Entity entity) {
		// check if the current entity exist
		assert(
			entity < MAX_ENTITIES &&
			"destroyEntity : Entity may be already destroyed or out of range.");

		// set all its bit field to 0
		mSignatures[entity].reset();

		// push our entity at the end of the queue
		mAvailableEntities.push(entity);
		--mLivingEntityCount;
	}

	void setSignature(Entity entity, Signature signature) {
		// check if the entity is in range
		assert(entity < MAX_ENTITIES &&
			   "SetSignatue : Entity may be out of range.");

		// assign signature to the entity
		mSignatures[entity] = signature;
	}

	Signature getSignature(Entity entity) {
		assert(entity < MAX_ENTITIES &&
			   "GetSignatue : Entity may be out of range.");
		return mSignatures[entity];
	}

  private:
	std::queue<Entity> mAvailableEntities{};
	std::array<Signature, MAX_ENTITIES> mSignatures{};
	uint32_t mLivingEntityCount{};
};
} // namespace ecs