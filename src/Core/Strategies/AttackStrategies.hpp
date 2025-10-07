/**
 * @file AttackStrategies.hpp
 * @brief Combat strategy system for entity attack behaviors and damage calculation.
 *
 * This file defines the attack strategy system that handles combat interactions
 * between entities in the battle simulation. The strategy pattern allows for
 * different attack types (melee, ranged) with varying damage, range, and
 * targeting mechanics.
 *
 * Key responsibilities:
 * - Attack behavior abstraction and polymorphism
 * - Damage calculation and application
 * - Range validation and targeting mechanics
 * - Combat event logging and tracking
 */

#pragma once

#include "../Types.hpp"

#include <memory>

namespace sw::core
{

	class Entity;
	class World;

	/**
	 * @brief Strategy interface for attack behaviors
	 *
	 * Defines the contract for all attack strategies in the simulation.
	 * Attack strategies handle the logic for entities attacking other entities,
	 * including damage calculation, range validation, and combat resolution.
	 *
	 * The strategy pattern allows for different attack types (melee, ranged, etc.)
	 * while maintaining a consistent interface for the simulation engine.
	 */
	class IAttackStrategy
	{
	public:
		IAttackStrategy() = default;
		virtual ~IAttackStrategy() noexcept = default;
		IAttackStrategy(const IAttackStrategy&) = delete;
		auto operator=(const IAttackStrategy&) -> IAttackStrategy& = delete;
		IAttackStrategy(IAttackStrategy&&) = delete;
		auto operator=(IAttackStrategy&&) -> IAttackStrategy& = delete;

		/**
		 * @brief Get the attack type for this strategy
		 * @return Attack type (melee, ranged, etc.)
		 */
		[[nodiscard]]
		virtual auto type() const -> AttackType
			= 0;

		/**
		 * @brief Perform attack logic against a target
		 * @param self The entity performing the attack
		 * @param target The entity being attacked
		 * @param world The world containing both entities
		 * @param turn Current turn number for event logging
		 * @return true if attack was successful, false otherwise
		 */
		virtual auto attack(Entity& self, Entity& target, World& world, TurnNumber turn) -> bool = 0;

		/**
		 * @brief Get damage value for this attack strategy
		 * @return Base damage amount for this attack
		 */
		[[nodiscard]]
		virtual auto damage() const -> DamageValue
			= 0;
	};

	/**
	 * @brief Concrete strategy for melee attacks
	 *
	 * Implements close-range combat where entities must be adjacent to attack.
	 * Melee attacks are typically strength-based and have no minimum range
	 * requirements, making them effective for close combat situations.
	 *
	 * Features:
	 * - Adjacent positioning requirement
	 * - Strength-based damage calculation
	 * - No range limitations (1 square range)
	 * - Direct combat engagement
	 */
	class MeleeAttackStrategy final : public IAttackStrategy
	{
	public:
		/**
		 * @brief Construct a melee attack strategy
		 * @param damage Base damage amount for the attack
		 */
		explicit MeleeAttackStrategy(DamageValue damage);

		/**
		 * @brief Get the attack type
		 * @return AttackType::Melee
		 */
		[[nodiscard]]
		constexpr auto type() const noexcept -> AttackType override
		{
			return AttackType::Melee;
		}

		/**
		 * @brief Perform a melee attack against the target
		 * @param self The entity performing the attack
		 * @param target The entity being attacked
		 * @param world The world containing both entities
		 * @param turn Current turn number for event logging
		 * @return true if attack was successful, false otherwise
		 */
		auto attack(Entity& self, Entity& target, World& world, TurnNumber turn) -> bool override;

		/**
		 * @brief Get the base damage for this melee attack
		 * @return Damage amount
		 */
		[[nodiscard]]
		constexpr auto damage() const noexcept -> DamageValue override
		{
			return _damage;
		}

	private:
		DamageValue _damage;  ///< Base damage amount for the melee attack
	};

	/**
	 * @brief Concrete strategy for ranged attacks
	 *
	 * Implements long-range combat where entities can attack from a distance.
	 * Ranged attacks are typically agility-based and have minimum and maximum
	 * range requirements, allowing for tactical positioning and kiting strategies.
	 *
	 * Features:
	 * - Configurable minimum and maximum range
	 * - Agility-based damage calculation
	 * - Optional clear adjacency requirements
	 * - Tactical positioning advantages
	 */
	class RangedAttackStrategy final : public IAttackStrategy
	{
	public:
		/**
		 * @brief Construct a ranged attack strategy
		 * @param damage Base damage amount for the attack
		 * @param minRange Minimum range required for the attack
		 * @param maxRange Maximum range for the attack
		 * @param requireClearAdjacency Whether attack requires clear adjacent squares
		 */
		struct RangeConfig
		{
			DamageValue damage{};
			RangeValue minRange{};
			RangeValue maxRange{};
			bool requireClearAdjacency = false;
		};

		RangedAttackStrategy(const RangeConfig& config);

		/**
		 * @brief Get the attack type
		 * @return AttackType::Ranged
		 */
		[[nodiscard]]
		constexpr auto type() const noexcept -> AttackType override
		{
			return AttackType::Ranged;
		}

		/**
		 * @brief Perform a ranged attack against the target
		 * @param self The entity performing the attack
		 * @param target The entity being attacked
		 * @param world The world containing both entities
		 * @param turn Current turn number for event logging
		 * @return true if attack was successful, false otherwise
		 */
		auto attack(Entity& self, Entity& target, World& world, TurnNumber turn) -> bool override;

		/**
		 * @brief Get the base damage for this ranged attack
		 * @return Damage amount
		 */
		[[nodiscard]]
		constexpr auto damage() const noexcept -> DamageValue override
		{
			return _damage;
		}

		// === Range Information ===

		/**
		 * @brief Get the minimum range for this ranged attack
		 * @return Minimum range in grid units
		 */
		[[nodiscard]]
		constexpr auto minRange() const noexcept -> RangeValue
		{
			return _minRange;
		}

		/**
		 * @brief Get the maximum range for this ranged attack
		 * @return Maximum range in grid units
		 */
		[[nodiscard]]
		constexpr auto maxRange() const noexcept -> RangeValue
		{
			return _maxRange;
		}

		/**
		 * @brief Check if this attack requires clear adjacent squares
		 * @return true if clear adjacency is required
		 */
		[[nodiscard]]
		constexpr auto requiresClearAdjacency() const noexcept -> bool
		{
			return _requireClearAdjacency;
		}

	private:
		DamageValue _damage;		  ///< Base damage amount for the ranged attack
		RangeValue _minRange;		  ///< Minimum range required for the attack
		RangeValue _maxRange;		  ///< Maximum range for the attack
		bool _requireClearAdjacency;  ///< Whether attack requires clear adjacent squares
	};

	/**
	 * @brief Factory function for creating melee attack strategies
	 * @param damage Base damage amount for the attack
	 * @return Unique pointer to melee attack strategy
	 */
	auto createMeleeAttack(DamageValue damage) -> std::unique_ptr<IAttackStrategy>;

	/**
	 * @brief Factory function for creating ranged attack strategies
	 * @param damage Base damage amount for the attack
	 * @param minRange Minimum range required for the attack
	 * @param maxRange Maximum range for the attack
	 * @param requireClearAdjacency Whether attack requires clear adjacent squares
	 * @return Unique pointer to ranged attack strategy
	 */
	auto createRangedAttack(
		DamageValue damage, RangeValue minRange, RangeValue maxRange, bool requireClearAdjacency = false)
		-> std::unique_ptr<IAttackStrategy>;

}  // namespace sw::core
