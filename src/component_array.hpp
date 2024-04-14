#pragma once

#include "ecs_type.hpp"

#include <array>
#include <cassert>
#include <unordered_map>

namespace ecs {

// Avoid typing conflict
class IComponentArray {
public:
  virtual ~IComponentArray() = default;
  virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T> class ComponentArray : public IComponentArray {

public:
  void InsertData(Entity entity, T component);
  void RemoveData(Entity entity);
  T &GetData(Entity entity);
  void EntityDestroyed(Entity entity);

private:
  std::array<T, MAX_ENTITIES> componentArray{};
  std::unordered_map<Entity, size_t> entityToIndex{};
  std::unordered_map<size_t, Entity> indexToEntity{};
  size_t currentSize{0};
};

} // namespace ecs