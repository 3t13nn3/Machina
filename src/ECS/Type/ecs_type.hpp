#pragma once

#include <bitset>
#include <cstdint>

namespace ecs {

using Entity = std::uint32_t;
using ComponentType = std::uint8_t;

constexpr Entity MAX_ENTITIES = 5000;
constexpr ComponentType MAX_COMPONENTS = 32;

constexpr Entity CAMERA_ENTITY = 0;
constexpr Entity LIGHT_CAMERA_ENTITY = 1;

using Signature = std::bitset<MAX_COMPONENTS>;

} // namespace ecs