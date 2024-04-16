#include "centralizer.hpp"

#include "../Type/template_type_macro.hpp"

COMPILE_FOR_COMPONENT(INSTANTIATE_CENTRALIZER_FOR_TYPE)
COMPILE_FOR_SYSTEM(INSTANTIATE_CENTRALIZER_FOR_SYSTEM)

namespace ecs {
Centralizer::Centralizer() {
	componentManager = std::make_unique<ComponentManager>();
	entityManager = std::make_unique<EntityManager>();
	systemManager = std::make_unique<SystemManager>();
}

Entity Centralizer::CreateEntity() { return entityManager->CreateEntity(); }

void Centralizer::DestroyEntity(Entity entity) {
	entityManager->DestroyEntity(entity);
	componentManager->EntityDestroyed(entity);
	systemManager->EntityDestroyed(entity);
}

template <typename T> void Centralizer::RegisterComponent() {
	componentManager->RegisterComponent<T>();
}

template <typename T>
void Centralizer::AddComponent(Entity entity, T component) {
	componentManager->AddComponent<T>(entity, component);

	Signature signature = entityManager->GetSignature(entity);
	signature.set(componentManager->GetComponentType<T>(), true);
	entityManager->SetSignature(entity, signature);

	systemManager->EntitySignatureChanged(entity, signature);
}

template <typename T> void Centralizer::RemoveComponent(Entity entity) {
	componentManager->RemoveComponent<T>(entity);

	Signature signature = entityManager->GetSignature(entity);
	signature.set(componentManager->GetComponentType<T>(), false);
	entityManager->SetSignature(entity, signature);

	systemManager->EntitySignatureChanged(entity, signature);
}

template <typename T> T &Centralizer::GetComponent(Entity entity) {
	return componentManager->GetComponent<T>(entity);
}

template <typename T> ComponentType Centralizer::GetComponentType() {
	return componentManager->GetComponentType<T>();
}

template <typename T> std::shared_ptr<T> Centralizer::RegisterSystem() {
	return systemManager->RegisterSystem<T>();
}

template <typename T>
void Centralizer::SetSystemSignature(Signature signature) {
	systemManager->SetSignature<T>(signature);
}
} // namespace ecs