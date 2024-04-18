#include "physics_system.hpp"

extern ecs::Centralizer gCentralizer;

namespace ecs {
void PhysicsSystem::Update() {
	for (const Entity &e : mEntities) {
		glm::vec3 &currentTransform =
			gCentralizer.GetComponent<Transform>(e).position;
		currentTransform += glm::vec3{3.1415, 1.0, 0.0};
	}
}
} // namespace ecs
