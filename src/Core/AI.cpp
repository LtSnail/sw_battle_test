#include "AI.hpp"

#include "Entity.hpp"
#include "World.hpp"

#include <algorithm>
#include <limits>
#include <random>
#include <ranges>
#include <vector>

namespace sw::core::detail
{
	auto randomEngine() -> std::mt19937&
	{
		static std::mt19937 engine(std::random_device{}());
		return engine;
	}

	void shuffleEnemies(std::vector<Entity*>& enemies)
	{
		auto& rng = randomEngine();
		std::ranges::shuffle(enemies, rng);
	}

	auto gatherEnemies(const Entity& self, World& world) -> std::vector<Entity*>
	{
		std::vector<Entity*> enemies;
		enemies.reserve(world.entities().size());
		for (auto& entity : world.entities() | std::views::values)
		{
			if (!entity || entity->id() == self.id() || !entity->isAlive())
			{
				continue;
			}
			enemies.push_back(entity.get());
		}

		shuffleEnemies(enemies);
		return enemies;
	}

	auto findNearestEnemy(const Entity& self, const std::vector<Entity*>& enemies) -> Entity*
	{
		Entity* nearest = nullptr;
		uint32_t bestDist = std::numeric_limits<uint32_t>::max();
		for (auto* enemy : enemies)
		{
			uint32_t dist = self.position().distanceTo(enemy->position());
			if (dist < bestDist)
			{
				bestDist = dist;
				nearest = enemy;
			}
		}
		return nearest;
	}
}
