/**
 * @file MovementStrategies.hpp
 * @brief Movement strategy system for entity locomotion and pathfinding.
 *
 * This file defines the movement strategy system that handles entity locomotion
 * and pathfinding within the battle simulation. The strategy pattern allows for
 * different movement behaviors while maintaining a consistent interface for
 * the simulation engine.
 *
 * Key responsibilities:
 * - Movement behavior abstraction and polymorphism
 * - Pathfinding and movement validation
 * - Collision detection and ground blocking
 * - Movement range and step size management
 */

#pragma once

#include "../Types.hpp"

#include <memory>

namespace sw::core
{

	class Entity;
	class World;

	/**
	 * @brief Strategy interface for movement behaviors
	 *
	 * Defines the contract for all movement strategies in the simulation.
	 * Movement strategies handle the logic for moving entities from one position
	 * to another, including pathfinding, collision detection, and movement validation.
	 *
	 * The strategy pattern allows for different movement types (terrain, flying, etc.)
	 * while maintaining a consistent interface for the simulation engine.
	 */
	class IMovementStrategy
	{
	public:
		IMovementStrategy() = default;
		virtual ~IMovementStrategy() noexcept = default;
		IMovementStrategy(const IMovementStrategy&) = delete;
		auto operator=(const IMovementStrategy&) -> IMovementStrategy& = delete;
		IMovementStrategy(IMovementStrategy&&) = delete;
		auto operator=(IMovementStrategy&&) -> IMovementStrategy& = delete;

		/**
		 * @brief Perform movement logic for an entity
		 * @param self The entity to move
		 * @param world The world containing the entity
		 * @param target Target position for movement
		 * @param turn Current turn number for event logging
		 * @return true if movement occurred, false otherwise
		 */
		virtual auto move(Entity& self, World& world, Position target, TurnNumber turn) -> bool = 0;

		/**
		 * @brief Whether this movement type blocks ground movement for other entities
		 * @return true if this movement type blocks ground, false otherwise
		 */
		[[nodiscard]]
		virtual auto blocksGround() const -> bool
			= 0;

		/**
		 * @brief Get the step size for this movement type
		 * @return Maximum distance this movement type can travel in one step
		 */
		[[nodiscard]]
		virtual auto stepSize() const -> RangeValue
			= 0;
	};

	/**
	 * @brief Concrete strategy for terrain-based movement
	 *
	 * Implements standard ground-based movement where entities move one step at a time
	 * towards their target, respecting terrain boundaries and collision detection.
	 * This is the most common movement type for ground-based units like swordsmen
	 * and hunters.
	 *
	 * Features:
	 * - One-step movement towards target
	 * - Collision detection with other entities
	 * - Ground blocking for other entities
	 * - Event logging for movement actions
	 */
	class TerrainMovementStrategy final : public IMovementStrategy
	{
	public:
		/**
		 * @brief Construct a terrain movement strategy
		 * @param step Step size for movement (default: 1)
		 */
		explicit TerrainMovementStrategy(RangeValue step = 1);

		/**
		 * @brief Move the entity one step towards the target
		 * @param self The entity to move
		 * @param world The world containing the entity
		 * @param target Target position for movement
		 * @param turn Current turn number for event logging
		 * @return true if movement occurred, false otherwise
		 */
		auto move(Entity& self, World& world, Position target, TurnNumber turn) -> bool override;

		/**
		 * @brief Terrain movement blocks ground for other entities
		 * @return true (terrain movement blocks ground)
		 */
		[[nodiscard]]
		constexpr auto blocksGround() const noexcept -> bool override
		{
			return true;
		}

		/**
		 * @brief Get the step size for terrain movement
		 * @return Step size in grid units
		 */
		[[nodiscard]]
		constexpr auto stepSize() const noexcept -> RangeValue override
		{
			return _step;
		}

	private:
		RangeValue _step;  ///< Maximum distance this movement can travel in one step
	};

	/**
	 * @brief Factory function for creating terrain movement strategies
	 * @param step Step size for movement (default: 1)
	 * @return Unique pointer to terrain movement strategy
	 */
	auto createTerrainMovement(RangeValue step = 1) -> std::unique_ptr<IMovementStrategy>;

}  // namespace sw::core
