#pragma once

#include "../Type/ecs_type.hpp"
#include "component_array.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>

namespace ecs {
class ComponentManager {

  public:
	template <typename T> void RegisterComponent();
	template <typename T> ComponentType GetComponentType();
	template <typename T> void AddComponent(Entity entity, T component);
	template <typename T> void RemoveComponent(Entity entity);
	template <typename T> T &GetComponent(Entity entity);
	void EntityDestroyed(Entity entity);

  private:
	// Component typename 1 - 1 to an "index" as ComponentType
	std::unordered_map<const char *, ComponentType> componentTypes{};
	// Component typename 1 - 1 to array of all this type of component
	std::unordered_map<const char *, std::shared_ptr<IComponentArray>>
		componentArrays{};
	ComponentType nextComponentType{};

	template <typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray();
};

} // namespace ecs