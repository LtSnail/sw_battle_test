#include "AttackStrategies.hpp"

#include "../Entity.hpp"
#include "../World.hpp"
#include "Core/Types.hpp"

#include <algorithm>
#include <memory>

namespace sw::core
{

	MeleeAttackStrategy::MeleeAttackStrategy(const DamageValue damage) :
			_damage(damage)
	{}

	auto MeleeAttackStrategy::attack(Entity& self, Entity& target, World& world, const TurnNumber turn) -> bool
	{
		if (!target.isAlive())
		{
			return false;
		}

		// Melee attacks can target any alive entity that's adjacent
		const Position a = self.position();
		if (const Position b = target.position(); a.distanceTo(b) != 1U)
		{
			return false;
		}

		world.applyDamage(self, target, {.damage = static_cast<int>(_damage), .turn = turn});
		return true;
	}

	RangedAttackStrategy::RangedAttackStrategy(const RangeConfig& config) :
			_damage(config.damage),
			_minRange(config.minRange),
			_maxRange(std::max(config.minRange, config.maxRange)),
			_requireClearAdjacency(config.requireClearAdjacency)
	{}

	namespace
	{
		auto hasClearAdjacency(const Entity& self, World& world) -> bool
		{
			const auto& map = world.map();
			const auto [x, y] = self.position();
			for (int dx = -1; dx <= 1; ++dx)
			{
				for (int dy = -1; dy <= 1; ++dy)
				{
					if (dx == 0 && dy == 0)
					{
						continue;
					}

					const int nx = static_cast<int>(x) + dx;
					const int ny = static_cast<int>(y) + dy;
					if (nx < 0 || ny < 0)
					{
						continue;
					}

					const Position neighbor(static_cast<uint32_t>(nx), static_cast<uint32_t>(ny));
					if (!map.isValidPosition(neighbor))
					{
						continue;
					}

					auto occupant = map.getUnitAt(neighbor);
					if (occupant.has_value() && *occupant != self.id())
					{
						if (const auto* occupantEntity = world.getEntity(*occupant);
							occupantEntity != nullptr && occupantEntity->isAlive())
						{
							return false;
						}
					}
				}
			}
			return true;
		}
	}

	auto RangedAttackStrategy::attack(Entity& self, Entity& target, World& world, TurnNumber turn) -> bool
	{
		if (!target.isAlive())
		{
			return false;
		}

		// Ranged attacks can target any alive entity within range
		// No tag-based restrictions - component-based inference only

		if (_requireClearAdjacency && !hasClearAdjacency(self, world))
		{
			return false;
		}

		// Check if target can be attacked by this attack type
		const auto targetHealth = target.health();
		if (targetHealth && !(*targetHealth)->canBeAttackedBy(type()))
		{
			return false;
		}

		uint32_t dist = self.position().distanceTo(target.position());
		RangeValue effectiveMinRange = _minRange;
		RangeValue effectiveMaxRange = _maxRange;

		// Modify range based on target's health strategy
		if (targetHealth)
		{
			effectiveMinRange = (*targetHealth)->getModifiedRange(_minRange, type());
			effectiveMaxRange = (*targetHealth)->getModifiedRange(_maxRange, type());
		}

		if (dist < effectiveMinRange || dist > effectiveMaxRange)
		{
			return false;
		}

		world.applyDamage(self, target, {.damage = static_cast<int>(_damage), .turn = turn});
		return true;
	}

	// Factory function implementations
	auto createMeleeAttack(DamageValue damage) -> std::unique_ptr<IAttackStrategy>
	{
		return std::make_unique<MeleeAttackStrategy>(damage);
	}

	auto createRangedAttack(
		const DamageValue damage,
		const RangeValue minRange,
		const RangeValue maxRange,
		const bool requireClearAdjacency) -> std::unique_ptr<IAttackStrategy>
	{
		return std::make_unique<RangedAttackStrategy>(RangedAttackStrategy::RangeConfig{
			.damage = damage,
			.minRange = minRange,
			.maxRange = maxRange,
			.requireClearAdjacency = requireClearAdjacency});
	}

}  // namespace sw::core
