#include "component_manager.hpp"

#include "../Type/template_type_macro.hpp"

COMPILE_FOR_COMPONENT(INSTANTIATE_COMPONENT_MANAGER_FOR_TYPE)

namespace ecs {

template <typename T> void ComponentManager::RegisterComponent() {
	// Recovering type name as a string
	const char *typeName = typeid(T).name();
	assert(componentTypes.find(typeName) == componentTypes.end() &&
		   "RegisterComponent : Type already registered.");
	componentTypes.insert({typeName, nextComponentType});
	// componentTypes[typeName] = nextComponentType;
	componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
	// componentArrays[typeName] = std::make_shared<ComponentArray<T>>();
	++nextComponentType;
}

template <typename T> ComponentType ComponentManager::GetComponentType() {
	const char *typeName = typeid(T).name();
	assert(componentTypes.find(typeName) != componentTypes.end() &&
		   "GetComponentType : Type not registered.");
	return componentTypes[typeName];
}

template <typename T>
void ComponentManager::AddComponent(Entity entity, T component) {
	GetComponentArray<T>()->InsertData(entity, component);
}

template <typename T> void ComponentManager::RemoveComponent(Entity entity) {
	GetComponentArray<T>()->RemoveData(entity);
}

template <typename T> T &ComponentManager::GetComponent(Entity entity) {
	return GetComponentArray<T>()->GetData(entity);
}

void ComponentManager::EntityDestroyed(Entity entity) {
	// For the current entity, delete in every component array tha data attached
	// to it
	for (const auto &e : componentArrays) {
		(e.second)->EntityDestroyed(entity);
	}
}

template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray() {
	const char *typeName = typeid(T).name();
	assert(componentTypes.find(typeName) != componentTypes.end() &&
		   "GetComponentArray : Component Type not registerd.");
	return std::static_pointer_cast<ComponentArray<T>>(
		componentArrays[typeName]);
}

} // namespace ecs