/**
 * @file HealthStrategies.hpp
 * @brief Health and vitality strategy system for entity damage and healing mechanics.
 *
 * This file defines the health strategy system that handles entity vitality,
 * damage application, and healing mechanics in the battle simulation. The
 * strategy pattern allows for different health systems while maintaining
 * a consistent interface for damage and healing operations.
 *
 * Key responsibilities:
 * - Health state management and tracking
 * - Damage application and healing mechanics
 * - Attack type validation and range modification
 * - Entity vitality state determination
 */

#pragma once

#include "../Types.hpp"

#include <memory>

namespace sw::core
{

	/**
	 * @brief Strategy interface for health and vitality behaviors
	 *
	 * Defines the contract for all health strategies in the simulation.
	 * Health strategies handle entity vitality, damage application, healing,
	 * and attack validation. This abstraction allows for different health
	 * systems (basic, regenerating, armored, etc.) while maintaining
	 * consistent combat mechanics.
	 */
	class IHealthStrategy
	{
	public:
		IHealthStrategy() = default;
		virtual ~IHealthStrategy() noexcept = default;
		IHealthStrategy(const IHealthStrategy&) = delete;
		auto operator=(const IHealthStrategy&) -> IHealthStrategy& = delete;
		IHealthStrategy(IHealthStrategy&&) = delete;
		auto operator=(IHealthStrategy&&) -> IHealthStrategy& = delete;

		/**
		 * @brief Check if the entity is alive
		 * @return true if the entity has positive health, false otherwise
		 */
		[[nodiscard]]
		virtual auto isAlive() const -> bool
			= 0;

		/**
		 * @brief Get the current hit points
		 * @return Current health points
		 */
		[[nodiscard]]
		virtual auto hitPoints() const -> HealthPoints
			= 0;

		/**
		 * @brief Apply damage to the entity
		 * @param amount Amount of damage to apply (positive value)
		 */
		virtual void applyDamage(int amount) = 0;

		/**
		 * @brief Heal the entity
		 * @param amount Amount of health to restore
		 */
		virtual void heal(HealthPoints amount) = 0;

		/**
		 * @brief Check if the entity can be attacked by a specific attack type
		 * @param attackType Type of attack to check
		 * @return true if the entity can be attacked by this attack type
		 */
		[[nodiscard]]
		virtual auto canBeAttackedBy(AttackType attackType) const -> bool
			= 0;

		/**
		 * @brief Get modified attack range based on health state
		 * @param originalRange Original attack range
		 * @param attackType Type of attack
		 * @return Modified attack range (maybe reduced by health effects)
		 */
		[[nodiscard]]
		virtual auto getModifiedRange(RangeValue originalRange, AttackType attackType) const -> RangeValue
			= 0;
	};

	/**
	 * @brief Concrete strategy for basic health mechanics
	 *
	 * Implements a simple health system where entities have a fixed amount of
	 * hit points that decrease when damaged and increase when healed. This is
	 * the standard health system used by most entities in the simulation.
	 *
	 * Features:
	 * - Fixed maximum health points
	 * - Simple damage and healing mechanics
	 * - Death when health reaches zero
	 * - No special resistances or immunities
	 */
	class BasicHealthStrategy final : public IHealthStrategy
	{
	public:
		/**
		 * @brief Construct a basic health strategy
		 * @param hp Initial hit points
		 */
		explicit BasicHealthStrategy(HealthPoints hp);

		/**
		 * @brief Check if the entity is alive
		 * @return true if hit points are greater than zero
		 */
		[[nodiscard]]
		auto isAlive() const -> bool override;

		/**
		 * @brief Get the current hit points
		 * @return Current health points
		 */
		[[nodiscard]]
		auto hitPoints() const -> HealthPoints override;

		/**
		 * @brief Apply damage to the entity
		 * @param amount Amount of damage to apply
		 */
		void applyDamage(int amount) override;

		/**
		 * @brief Heal the entity
		 * @param amount Amount of health to restore
		 */
		void heal(HealthPoints amount) override;

		/**
		 * @brief Check if the entity can be attacked by any attack type
		 * @param attackType Type of attack to check
		 * @return true (basic health has no immunities)
		 */
		[[nodiscard]]
		auto canBeAttackedBy(AttackType attackType) const -> bool override;

		/**
		 * @brief Get unmodified attack range
		 * @param originalRange Original attack range
		 * @param attackType Type of attack
		 * @return Original range (basic health doesn't modify range)
		 */
		[[nodiscard]]
		auto getModifiedRange(RangeValue originalRange, AttackType attackType) const -> RangeValue override;

	private:
		int _hp;  ///< Current hit points (can go negative)
	};

	/**
	 * @brief Factory function for creating basic health strategies
	 * @param hp Initial hit points
	 * @return Unique pointer to basic health strategy
	 */
	auto createBasicHealth(HealthPoints hp) -> std::unique_ptr<IHealthStrategy>;
}
