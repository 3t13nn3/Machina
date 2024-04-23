#pragma once

#include "../Type/ecs_type.hpp"

#include <array>
#include <cassert>
#include <iostream>
#include <unordered_map>

namespace ecs {

// Avoid typing conflict
class IComponentArray {
  public:
	virtual ~IComponentArray() = default;
	virtual void entityDestroyed(Entity entity) = 0;
};

template <typename T> class ComponentArray : public IComponentArray {

  public:
	void insertData(Entity entity, T component) {
		assert(mEntityToIndex.find(entity) == mEntityToIndex.end() &&
			   "insertData : Entity is already in the component array.");

		// keep the index to data position
		mEntityToIndex[entity] = currentSize;
		mIndexToEntity[currentSize] = entity;

		// add the component to the right place
		mComponentArray[currentSize] = component;

		++currentSize;
	}

	void removeData(Entity entity) {
		assert(mEntityToIndex.find(entity) != mEntityToIndex.end() &&
			   "removeData : Entity isn't in the component array.");

		// put the last element of the array at the "to remove" entity index
		size_t removedEntityIndex = mEntityToIndex[entity];
		size_t LastEntityIndex = currentSize - 1;
		mComponentArray[removedEntityIndex] = mComponentArray[LastEntityIndex];

		// update map to keep the 1-1 relation between the new index and entity
		Entity lastElement = mIndexToEntity[removedEntityIndex];
		mEntityToIndex[lastElement] = removedEntityIndex;
		mIndexToEntity[removedEntityIndex] = lastElement;

		--currentSize;
	}

	T &getData(Entity entity) {
		assert(mEntityToIndex.find(entity) != mEntityToIndex.end() &&
			   "getData : Entity isn't in the component array.");

		return mComponentArray[mEntityToIndex[entity]];
	}

	void entityDestroyed(Entity entity) { removeData(entity); }

  private:
	std::array<T, MAX_ENTITIES> mComponentArray{};
	std::unordered_map<Entity, size_t> mEntityToIndex{};
	std::unordered_map<size_t, Entity> mIndexToEntity{};
	size_t currentSize{0};
};

} // namespace ecs