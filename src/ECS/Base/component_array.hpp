#pragma once

#include "../Type/ecs_type.hpp"

#include <array>
#include <cassert>
#include <unordered_map>

namespace ecs {

// Avoid typing conflict
class IComponentArray {
  public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T> class ComponentArray : public IComponentArray {

  public:
	void InsertData(Entity entity, T component) {
		assert(entityToIndex.find(entity) == entityToIndex.end() &&
			   "InsertData : Entity is already in the component array.");

		// keep the index to data position
		entityToIndex[entity] = currentSize;
		indexToEntity[currentSize] = entity;

		// add the component to the right place
		componentArray[currentSize] = component;

		++currentSize;
	}

	void RemoveData(Entity entity) {
		assert(entityToIndex.find(entity) != entityToIndex.end() &&
			   "RemoveData : Entity isn't in the component array.");

		// put the last element of the array at the "to remove" entity index
		size_t removedEntityIndex = entityToIndex[entity];
		size_t LastEntityIndex = currentSize - 1;
		componentArray[removedEntityIndex] = componentArray[LastEntityIndex];

		// update map to keep the 1-1 relation between the new index and entity
		Entity lastElement = indexToEntity[removedEntityIndex];
		entityToIndex[lastElement] = removedEntityIndex;
		indexToEntity[removedEntityIndex] = lastElement;

		--currentSize;
	}

	T &GetData(Entity entity) {
		assert(entityToIndex.find(entity) != entityToIndex.end() &&
			   "GetData : Entity isn't in the component array.");

		return componentArray[entityToIndex[entity]];
	}

	void EntityDestroyed(Entity entity) { RemoveData(entity); }

  private:
	std::array<T, MAX_ENTITIES> componentArray{};
	std::unordered_map<Entity, size_t> entityToIndex{};
	std::unordered_map<size_t, Entity> indexToEntity{};
	size_t currentSize{0};
};

} // namespace ecs