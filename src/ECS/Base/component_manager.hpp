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
		assert(componentTypes.find(typeName) == componentTypes.end() &&
			   "RegisterComponent : Type already registered.");
		componentTypes.insert({typeName, nextComponentType});
		// componentTypes[typeName] = nextComponentType;
		componentArrays.insert(
			{typeName, std::make_shared<ComponentArray<T>>()});
		// componentArrays[typeName] = std::make_shared<ComponentArray<T>>();
		++nextComponentType;
	}

	template <typename T> ComponentType GetComponentType() {
		const char *typeName = typeid(T).name();
		assert(componentTypes.find(typeName) != componentTypes.end() &&
			   "GetComponentType : Type not registered.");
		return componentTypes[typeName];
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
		for (const auto &e : componentArrays) {
			(e.second)->EntityDestroyed(entity);
		}
	}

  private:
	// Component typename 1 - 1 to an "index" as ComponentType
	std::unordered_map<const char *, ComponentType> componentTypes{};
	// Component typename 1 - 1 to array of all this type of component
	std::unordered_map<const char *, std::shared_ptr<IComponentArray>>
		componentArrays{};
	ComponentType nextComponentType{};

	template <typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray() {
		const char *typeName = typeid(T).name();
		assert(componentTypes.find(typeName) != componentTypes.end() &&
			   "GetComponentArray : Component Type not registerd.");
		return std::static_pointer_cast<ComponentArray<T>>(
			componentArrays[typeName]);
	}
};

} // namespace ecs