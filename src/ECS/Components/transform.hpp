#pragma once

#include <glm/vec3.hpp>

namespace ecs {

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

} // namespace ecs