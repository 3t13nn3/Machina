#include "physics_system.hpp"

extern ecs::Centralizer c;

namespace ecs {
void PhysicsSystem::Update() {
	for (const Entity &e : entities) {
		glm::vec3 &currentTransform = c.GetComponent<Transform>(e).position;
		currentTransform += glm::vec3{3.1415, 1.0, 0.0};
	}
}
} // namespace ecs
