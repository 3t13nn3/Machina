#include "camera_system.hpp"

extern std::unique_ptr<ecs::Centralizer> gCentralizer;

namespace ecs {

void CameraSystem::update(GlobalUbo &ubo, float aspect) {
	for (const Entity &e : mEntities) {
		setViewYXZ();
		setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

		auto &cam = gCentralizer->getComponent<ecs::Camera>(e);
		ubo.projection = cam.projectionMatrix;
		ubo.view = cam.viewMatrix;
		ubo.inverseView = cam.inverseViewMatrix;
	}
}

void CameraSystem::setPerspectiveProjection(float fovy, float aspect,
											float near, float far) {
	assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
	assert(mEntities.size() <= 1 &&
		   "The system only handle 1 Camera. Can't have more.");

	for (const Entity &e : mEntities) {
		auto &cam = gCentralizer->getComponent<ecs::Camera>(e);
		const float tanHalfFovy = tan(fovy / 2.f);
		cam.projectionMatrix = glm::mat4{0.0f};
		cam.projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
		cam.projectionMatrix[1][1] = 1.f / (tanHalfFovy);
		cam.projectionMatrix[2][2] = far / (far - near);
		cam.projectionMatrix[2][3] = 1.f;
		cam.projectionMatrix[3][2] = -(far * near) / (far - near);
	}
}

void CameraSystem::setViewYXZ() {
	assert(mEntities.size() <= 1 &&
		   "The system only handle 1 Camera. Can't have more.");
	for (const Entity &e : mEntities) {
		auto &transform = gCentralizer->getComponent<ecs::Transform>(e);

		const float c3 = glm::cos(transform.rotation.z);
		const float s3 = glm::sin(transform.rotation.z);
		const float c2 = glm::cos(transform.rotation.x);
		const float s2 = glm::sin(transform.rotation.x);
		const float c1 = glm::cos(transform.rotation.y);
		const float s1 = glm::sin(transform.rotation.y);
		const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3),
						  (c1 * s2 * s3 - c3 * s1)};
		const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3),
						  (c1 * c3 * s2 + s1 * s3)};
		const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};

		auto &cam = gCentralizer->getComponent<ecs::Camera>(CAMERA_ENTITY);
		cam.viewMatrix = glm::mat4{1.f};
		cam.viewMatrix[0][0] = u.x;
		cam.viewMatrix[1][0] = u.y;
		cam.viewMatrix[2][0] = u.z;
		cam.viewMatrix[0][1] = v.x;
		cam.viewMatrix[1][1] = v.y;
		cam.viewMatrix[2][1] = v.z;
		cam.viewMatrix[0][2] = w.x;
		cam.viewMatrix[1][2] = w.y;
		cam.viewMatrix[2][2] = w.z;
		cam.viewMatrix[3][0] = -glm::dot(u, transform.position);
		cam.viewMatrix[3][1] = -glm::dot(v, transform.position);
		cam.viewMatrix[3][2] = -glm::dot(w, transform.position);

		cam.inverseViewMatrix = glm::mat4{1.f};
		cam.inverseViewMatrix[0][0] = u.x;
		cam.inverseViewMatrix[0][1] = u.y;
		cam.inverseViewMatrix[0][2] = u.z;
		cam.inverseViewMatrix[1][0] = v.x;
		cam.inverseViewMatrix[1][1] = v.y;
		cam.inverseViewMatrix[1][2] = v.z;
		cam.inverseViewMatrix[2][0] = w.x;
		cam.inverseViewMatrix[2][1] = w.y;
		cam.inverseViewMatrix[2][2] = w.z;
		cam.inverseViewMatrix[3][0] = transform.position.x;
		cam.inverseViewMatrix[3][1] = transform.position.y;
		cam.inverseViewMatrix[3][2] = transform.position.z;
	}
}

} // namespace ecs
