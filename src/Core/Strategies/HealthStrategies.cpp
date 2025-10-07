#include "HealthStrategies.hpp"

#include "Core/Types.hpp"

#include <algorithm>
#include <memory>

namespace sw::core
{

	// BasicHealthStrategy implementation
	BasicHealthStrategy::BasicHealthStrategy(HealthPoints hp) :
			_hp(static_cast<int>(hp))
	{}

	auto BasicHealthStrategy::isAlive() const -> bool
	{
		return _hp > 0;
	}

	auto BasicHealthStrategy::hitPoints() const -> HealthPoints
	{
		return _hp > 0 ? static_cast<HealthPoints>(_hp) : 0U;
	}

	void BasicHealthStrategy::applyDamage(int amount)
	{
		if (amount <= 0)
		{
			return;
		}
		_hp = std::max(0, _hp - amount);
	}

	void BasicHealthStrategy::heal(HealthPoints amount)
	{
		if (amount == 0U)
		{
			return;
		}
		_hp += static_cast<int>(amount);
	}

	auto BasicHealthStrategy::canBeAttackedBy(AttackType attackType) const -> bool
	{
		// Basic health can be attacked by any attack type
		return true;
	}

	auto BasicHealthStrategy::getModifiedRange(RangeValue originalRange, AttackType attackType) const -> RangeValue
	{
		// Basic health doesn't modify attack ranges
		return originalRange;
	}

	// Factory function implementations
	auto createBasicHealth(HealthPoints hp) -> std::unique_ptr<IHealthStrategy>
	{
		return std::make_unique<BasicHealthStrategy>(hp);
	}

}
