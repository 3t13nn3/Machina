#pragma once

#include <glm/vec3.hpp>

#include <memory>

#include "../../vulkan/model.hpp"

namespace ecs {

struct Model {
	std::shared_ptr<vu::Model> model{};
};

} // namespace ecs