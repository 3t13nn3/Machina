#pragma once

#include "../Type/ecs_type.hpp"
#include "component_array.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>

namespace ecs {
class ComponentManager {

  public:
	template <typename T> void RegisterComponent() {
		// Recovering type name as a string
		const char *typeName = typeid(T).name();
		assert(mComponentTypes.find(typeName) == mComponentTypes.end() &&
			   "RegisterComponent : Type already registered.");
		mComponentTypes.insert({typeName, mNextComponentType});
		// mComponentTypes[typeName] = mNextComponentType;
		mComponentArrays.insert(
			{typeName, std::make_shared<ComponentArray<T>>()});
		// mComponentArrays[typeName] = std::make_shared<ComponentArray<T>>();
		++mNextComponentType;
	}

	template <typename T> ComponentType GetComponentType() {
		const char *typeName = typeid(T).name();
		assert(mComponentTypes.find(typeName) != mComponentTypes.end() &&
			   "GetComponentType : Type not registered.");
		return mComponentTypes[typeName];
	}

	template <typename T> void AddComponent(Entity entity, T component) {
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template <typename T> void RemoveComponent(Entity entity) {
		GetComponentArray<T>()->RemoveData(entity);
	}

	template <typename T> T &GetComponent(Entity entity) {
		return GetComponentArray<T>()->GetData(entity);
	}

	void EntityDestroyed(Entity entity) {
		// For the current entity, delete in every component array tha data
		// attached to it
		for (const auto &e : mComponentArrays) {
			(e.second)->EntityDestroyed(entity);
		}
	}

  private:
	// Component typename 1 - 1 to an "index" as ComponentType
	std::unordered_map<const char *, ComponentType> mComponentTypes{};
	// Component typename 1 - 1 to array of all this type of component
	std::unordered_map<const char *, std::shared_ptr<IComponentArray>>
		mComponentArrays{};
	ComponentType mNextComponentType{};

	template <typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray() {
		const char *typeName = typeid(T).name();
		assert(mComponentTypes.find(typeName) != mComponentTypes.end() &&
			   "GetComponentArray : Component Type not registerd.");
		return std::static_pointer_cast<ComponentArray<T>>(
			mComponentArrays[typeName]);
	}
};

} // namespace ecs