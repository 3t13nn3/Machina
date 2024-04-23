#pragma once

#include "../Type/ecs_type.hpp"
#include "component_array.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>

namespace ecs {
class ComponentManager {

  public:
	template <typename T> void registerComponent() {
		// Recovering type name as a string
		const char *typeName = typeid(T).name();
		assert(mComponentTypes.find(typeName) == mComponentTypes.end() &&
			   "registerComponent : Type already registered.");
		mComponentTypes.insert({typeName, mNextComponentType});
		// mComponentTypes[typeName] = mNextComponentType;
		mComponentArrays.insert(
			{typeName, std::make_shared<ComponentArray<T>>()});
		// mComponentArrays[typeName] = std::make_shared<ComponentArray<T>>();
		++mNextComponentType;
	}

	template <typename T> ComponentType getComponentType() {
		const char *typeName = typeid(T).name();
		assert(mComponentTypes.find(typeName) != mComponentTypes.end() &&
			   "getComponentType : Type not registered.");
		return mComponentTypes[typeName];
	}

	template <typename T> void addComponent(Entity entity, T component) {
		getComponentArray<T>()->insertData(entity, component);
	}

	template <typename T> void removeComponent(Entity entity) {
		getComponentArray<T>()->removeData(entity);
	}

	template <typename T> T &getComponent(Entity entity) {
		return getComponentArray<T>()->getData(entity);
	}

	void entityDestroyed(Entity entity) {
		// For the current entity, delete in every component array tha data
		// attached to it
		for (const auto &e : mComponentArrays) {
			(e.second)->entityDestroyed(entity);
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
	std::shared_ptr<ComponentArray<T>> getComponentArray() {
		const char *typeName = typeid(T).name();
		assert(mComponentTypes.find(typeName) != mComponentTypes.end() &&
			   "getComponentArray : Component Type not registerd.");
		return std::static_pointer_cast<ComponentArray<T>>(
			mComponentArrays[typeName]);
	}
};

} // namespace ecs