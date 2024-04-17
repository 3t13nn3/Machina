#pragma once

#include "component_manager.hpp"
#include "entity_manager.hpp"
#include "system_manager.hpp"

#include "../Type/ecs_type.hpp"

#include <memory>

namespace ecs {
class Centralizer {
  public:
	Centralizer() {
		componentManager = std::make_unique<ComponentManager>();
		entityManager = std::make_unique<EntityManager>();
		systemManager = std::make_unique<SystemManager>();
	}

	Entity CreateEntity() { return entityManager->CreateEntity(); }

	void DestroyEntity(Entity entity) {
		entityManager->DestroyEntity(entity);
		componentManager->EntityDestroyed(entity);
		systemManager->EntityDestroyed(entity);
	}

	template <typename T> void RegisterComponent() {
		componentManager->RegisterComponent<T>();
	}

	template <typename T> void AddComponent(Entity entity, T component) {
		componentManager->AddComponent<T>(entity, component);

		Signature signature = entityManager->GetSignature(entity);
		signature.set(componentManager->GetComponentType<T>(), true);
		entityManager->SetSignature(entity, signature);

		systemManager->EntitySignatureChanged(entity, signature);
	}

	template <typename T> void RemoveComponent(Entity entity) {
		componentManager->RemoveComponent<T>(entity);

		Signature signature = entityManager->GetSignature(entity);
		signature.set(componentManager->GetComponentType<T>(), false);
		entityManager->SetSignature(entity, signature);

		systemManager->EntitySignatureChanged(entity, signature);
	}

	template <typename T> T &GetComponent(Entity entity) {
		return componentManager->GetComponent<T>(entity);
	}

	template <typename T> ComponentType GetComponentType() {
		return componentManager->GetComponentType<T>();
	}

	template <typename T> std::shared_ptr<T> RegisterSystem() {
		return systemManager->RegisterSystem<T>();
	}

	template <typename T> void SetSystemSignature(Signature signature) {
		systemManager->SetSignature<T>(signature);
	}

  private:
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<SystemManager> systemManager;
};
} // namespace ecs