// #include "ECS/ECS.hpp"

// #include <iostream>

// ecs::Centralizer c{};

// int main() {

// 	c.registerComponent<ecs::Transform>();

// 	std::shared_ptr<ecs::PhysicsSystem> sys =
// 		c.registerSystem<ecs::PhysicsSystem>();

// 	ecs::Signature sign;
// 	sign.set(c.getComponentType<ecs::Transform>());

// 	c.setSystemSignature<ecs::PhysicsSystem>(sign);

// 	std::vector<ecs::Entity> entities(ecs::MAX_ENTITIES);
// 	for (auto &e : entities) {
// 		e = c.createEntity();

// 		c.addComponent(e, ecs::Transform{});
// 	}

// 	while (true) {
// 		sys->Update();
// 		std::cout << c.getComponent<ecs::Transform>(0).position.x << std::endl;
// 	}

// 	std::cout << "Machina !" << std::endl;

// 	return 0;
// }

#include "app.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "ECS/ECS.hpp"

std::unique_ptr<ecs::Centralizer> gCentralizer{};

int main() {

	gCentralizer = std::make_unique<ecs::Centralizer>();

	vu::App app{};

	try {
		app.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}