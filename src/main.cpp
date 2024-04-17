// #include "ECS/ECS.hpp"

// #include <iostream>

// ecs::Centralizer c{};

// int main() {

// 	c.RegisterComponent<ecs::Transform>();

// 	std::shared_ptr<ecs::PhysicsSystem> sys =
// 		c.RegisterSystem<ecs::PhysicsSystem>();

// 	ecs::Signature sign;
// 	sign.set(c.GetComponentType<ecs::Transform>());

// 	c.SetSystemSignature<ecs::PhysicsSystem>(sign);

// 	std::vector<ecs::Entity> entities(ecs::MAX_ENTITIES);
// 	for (auto &e : entities) {
// 		e = c.CreateEntity();

// 		c.AddComponent(e, ecs::Transform{});
// 	}

// 	while (true) {
// 		sys->Update();
// 		std::cout << c.GetComponent<ecs::Transform>(0).position.x << std::endl;
// 	}

// 	std::cout << "Machina !" << std::endl;

// 	return 0;
// }

#include "vulkan/first_app.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "ECS/ECS.hpp"

ecs::Centralizer c{};

int main() {
	vu::FirstApp app{};

	try {
		app.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}