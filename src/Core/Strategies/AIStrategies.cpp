#include "AIStrategies.hpp"

#include "../Entity.hpp"
#include "../World.hpp"
#include "Core/Types.hpp"

#include <algorithm>
#include <limits>
#include <memory>
#include <random>
#include <ranges>
#include <vector>

namespace sw::core
{
	namespace detail
	{
		inline auto randomEngine() -> std::mt19937&
		{
			static std::mt19937 engine(std::random_device{}());
			return engine;
		}

		inline void shuffleEnemies(std::vector<Entity*>& enemies)
		{
			auto& rng = randomEngine();
			std::ranges::shuffle(enemies, rng);
		}

		inline auto gatherEnemies(const Entity& self, World& world) -> std::vector<Entity*>
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

		inline auto findNearestEnemy(const Entity& self, const std::vector<Entity*>& enemies) -> Entity*
		{
			Entity* nearest = nullptr;
			uint32_t bestDist = std::numeric_limits<uint32_t>::max();
			for (auto* enemy : enemies)
			{
				if (const uint32_t dist = self.position().distanceTo(enemy->position()); dist < bestDist)
				{
					bestDist = dist;
					nearest = enemy;
				}
			}
			return nearest;
		}
	}

	auto SwordsmanAIStrategy::update(Entity& self, World& world, const TurnNumber turn) -> bool
	{
		const auto enemies = detail::gatherEnemies(self, world);
		for (auto* enemy : enemies)
		{
			if (world.executeAttack(self, *enemy, turn, AttackType::Melee))
			{
				return true;
			}
		}

		if (const Entity* nearest = detail::findNearestEnemy(self, enemies))
		{
			if (world.moveEntityTowards(self, nearest->position(), turn))
			{
				return true;
			}
		}

		return false;
	}

	auto HunterAIStrategy::update(Entity& self, World& world, const TurnNumber turn) -> bool
	{
		auto enemies = detail::gatherEnemies(self, world);
		for (auto* enemy : enemies)
		{
			if (world.executeAttack(self, *enemy, turn, AttackType::Ranged))
			{
				return true;
			}
		}

		for (auto* enemy : enemies)
		{
			if (world.executeAttack(self, *enemy, turn, AttackType::Melee))
			{
				return true;
			}
		}

		if (const Entity* nearest = detail::findNearestEnemy(self, enemies))
		{
			if (world.moveEntityTowards(self, nearest->position(), turn))
			{
				return true;
			}
		}

		return false;
	}

	// Factory function implementations
	auto createSwordsmanAI() -> std::unique_ptr<IAIStrategy>
	{
		return std::make_unique<SwordsmanAIStrategy>();
	}

	auto createHunterAI() -> std::unique_ptr<IAIStrategy>
	{
		return std::make_unique<HunterAIStrategy>();
	}

}  // namespace sw::core
