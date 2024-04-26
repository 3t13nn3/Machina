#include "collision_system.hpp"

extern std::unique_ptr<ecs::Centralizer> gCentralizer;

namespace ecs {

CollisionSystem::CollisionSystem() {}

void CollisionSystem::update(FrameInfo &frameInfo) {
  std::cout << mEntities.size() << std::endl;
  for (const Entity &e : mEntities) {
    auto &gravity = gCentralizer->getComponent<ecs::Gravity>(e);
    auto &rigidBody = gCentralizer->getComponent<ecs::RigidBody>(e);
    auto &transform = gCentralizer->getComponent<ecs::Transform>(e);

    rigidBody.acceleration += gravity.force * rigidBody.mass;
    rigidBody.velocity += rigidBody.acceleration * frameInfo.frameTime;
    transform.position += rigidBody.velocity * frameInfo.frameTime;
    // rigidBody.acceleration = glm::vec3{0.f, 0.f, 0.f};
  }
}

} // namespace ecs
