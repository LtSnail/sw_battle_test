/**
 * @file Entity.hpp
 * @brief Core entity class representing game units with component-based architecture.
 *
 * The Entity class serves as the fundamental unit in the battle simulation system.
 * It uses a component-based design pattern where different aspects of behavior
 * (health, movement, combat, AI) are encapsulated in strategy objects. This design
 * provides flexibility and extensibility for different unit types while maintaining
 * a clean separation of concerns.
 *
 * Key responsibilities:
 * - Identity management (ID, type, position)
 * - Component composition and delegation
 * - Behavioral state inference from components
 */

#pragma once

#include "Strategies/AIStrategies.hpp"
#include "Strategies/AttackStrategies.hpp"
#include "Strategies/HealthStrategies.hpp"
#include "Strategies/MovementStrategies.hpp"
#include "Types.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sw::core
{

	class World;

	/**
	 * @brief Core entity class representing game units with component-based architecture
	 *
	 * Entities are the fundamental units in the battle simulation. Each entity has a unique
	 * ID, type name, and position, along with optional behavioral components that define
	 * its capabilities and behavior. The component-based design allows for flexible
	 * composition of different unit types without inheritance hierarchies.
	 *
	 * Components:
	 * - Health: Manages HP and vitality state
	 * - Movement: Defines movement capabilities and pathfinding
	 * - Attacks: Collection of available attack methods
	 * - AI: Controls autonomous decision-making behavior
	 *
	 * The entity uses composition over inheritance, allowing for flexible unit creation
	 * through the Prefabs system while maintaining clean separation of concerns.
	 */
	class Entity
	{
	public:
		/**
		 * @brief Construct an entity with basic identity information
		 * @param id Unique identifier for this entity
		 * @param position Initial position on the map
		 * @param typeName Human-readable type name (e.g., "Swordsman", "Hunter")
		 */
		Entity(UnitId id, Position position, std::string typeName);

		// === Identity Management ===

		/**
		 * @brief Get the unique identifier for this entity
		 * @return Unit ID
		 */
		[[nodiscard]]
		constexpr auto id() const noexcept -> UnitId
		{
			return _id;
		}

		/**
		 * @brief Get the type name of this entity
		 * @return Type name string
		 */
		[[nodiscard]]
		auto typeName() const noexcept -> const std::string&
		{
			return _typeName;
		}

		/**
		 * @brief Get the current position of this entity
		 * @return Current position on the map
		 */
		[[nodiscard]]
		constexpr auto position() const noexcept -> Position
		{
			return _position;
		}

		/**
		 * @brief Set the position of this entity
		 * @param position New position on the map
		 */
		constexpr void setPosition(Position position) noexcept
		{
			_position = position;
		}

		// === Component Management ===

		/**
		 * @brief Set the health strategy component
		 * @param health Unique pointer to health strategy
		 */
		void setHealth(std::unique_ptr<IHealthStrategy> health);

		/**
		 * @brief Get the health strategy component
		 * @return Optional pointer to health strategy, or nullopt if not set
		 */
		[[nodiscard]]
		auto health() const -> std::optional<IHealthStrategy*>;

		/**
		 * @brief Set the movement strategy component
		 * @param movement Unique pointer to movement strategy
		 */
		void setMovement(std::unique_ptr<IMovementStrategy> movement);

		/**
		 * @brief Get the movement strategy component
		 * @return Optional pointer to movement strategy, or nullopt if not set
		 */
		[[nodiscard]]
		auto movement() const -> std::optional<IMovementStrategy*>;

		/**
		 * @brief Add an attack strategy component
		 * @param attack Unique pointer to attack strategy
		 */
		void addAttack(std::unique_ptr<IAttackStrategy> attack);

		/**
		 * @brief Get all attack strategy components
		 * @return Const reference to vector of attack strategies
		 */
		[[nodiscard]]
		auto attacks() const -> const std::vector<std::unique_ptr<IAttackStrategy>>&;

		/**
		 * @brief Set the AI strategy component
		 * @param ai Unique pointer to AI strategy
		 */
		void setAI(std::unique_ptr<IAIStrategy> ai);

		/**
		 * @brief Get the AI strategy component
		 * @return Optional pointer to AI strategy, or nullopt if not set
		 */
		[[nodiscard]]
		auto ai() const -> std::optional<IAIStrategy*>;

		// === Behavioral State Inference ===

		/**
		 * @brief Check if the entity is alive based on health component
		 * @return true if entity has health component and is alive
		 */
		[[nodiscard]]
		auto isAlive() const -> bool;

		/**
		 * @brief Check if the entity blocks ground movement
		 * @return true if entity has health component (alive entities block ground)
		 */
		[[nodiscard]]
		auto blocksGround() const -> bool;

		/**
		 * @brief Check if the entity can move
		 * @return true if entity has movement component
		 */
		[[nodiscard]]
		constexpr auto canMove() const noexcept -> bool;

	private:
		// === Core Identity Data ===
		UnitId _id;				///< Unique identifier for this entity
		Position _position;		///< Current position on the map
		std::string _typeName;	///< Human-readable type name

		// === Behavioral Components ===
		std::optional<std::unique_ptr<IHealthStrategy>> _health;	  ///< Health and vitality management
		std::optional<std::unique_ptr<IMovementStrategy>> _movement;  ///< Movement capabilities
		std::vector<std::unique_ptr<IAttackStrategy>> _attacks;		  ///< Available attack methods
		std::optional<std::unique_ptr<IAIStrategy>> _ai;			  ///< Autonomous decision-making
	};

}
