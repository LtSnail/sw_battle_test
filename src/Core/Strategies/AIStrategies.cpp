#include "AIStrategies.hpp"

#include "../AI.hpp"
#include "../Entity.hpp"
#include "../Types.hpp"
#include "../World.hpp"

#include <memory>

namespace sw::core
{

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
