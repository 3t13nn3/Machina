#pragma once

#include "Type/ecs_type.hpp"

#include "Base/centralizer.hpp"
#include "Base/component_array.hpp"
#include "Base/component_manager.hpp"
#include "Base/entity_manager.hpp"
#include "Base/system.hpp"
#include "Base/system_manager.hpp"

#include "Systems/camera_system.hpp"
#include "Systems/gravity_system.hpp"
#include "Systems/point_light_system.hpp"
#include "Systems/render_system.hpp"
#include "Systems/simple_render_system.hpp"

#include "Components/camera.hpp"
#include "Components/color.hpp"
#include "Components/gravity.hpp"
#include "Components/model.hpp"
#include "Components/point_light.hpp"
#include "Components/rigid_body.hpp"
#include "Components/transform.hpp"
