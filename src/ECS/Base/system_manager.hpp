#pragma once

#include "../Type/ecs_type.hpp"
#include "system.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>

namespace ecs {
class SystemManager {
  public:
	template <typename T, typename... Args>
	std::shared_ptr<T> registerSystem(Args &&...args) {
		const char *typeName = typeid(T).name();
		assert(mSystems.find(typeName) == mSystems.end() &&
			   "registerSystem : System already exists.");

		auto sys = std::make_shared<T>(
			std::forward<Args>(args)...); // Pass arguments to constructor
		mSystems.insert({typeName, sys});

		return sys;
	}

	template <typename T> void setSignature(Signature signature) {
		const char *typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() &&
			   "setSignature : System doesn't exist.");

		// if we want to update the signature, maybe use [] operator
		mSignatures.insert({typeName, signature});
	}

	void entityDestroyed(Entity entity) {
		// Erase the entity for each systems
		for (const auto &e : mSystems) {
			(e.second)->mEntities.erase(entity);
		}
	}

	void entitySignatureChanged(Entity entity, Signature entitySignature) {
		// browse every system
		for (const auto &e : mSystems) {
			auto const &type = e.first;
			auto const &system = e.second;
			auto const &systemSignature = mSignatures[type];

			// Check if the entity got the bit for the current system
			if ((entitySignature & systemSignature) == systemSignature) {
				system->mEntities.insert(entity);
			} else { // CHECK
				system->mEntities.erase(entity);
			}
		}
	}

  private:
	std::unordered_map<const char *, Signature> mSignatures{};
	std::unordered_map<const char *, std::shared_ptr<System>> mSystems{};
};
} // namespace ecs