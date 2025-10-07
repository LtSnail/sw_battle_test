/**
 * @file AIStrategies.hpp
 * @brief Artificial Intelligence strategy system for autonomous entity behavior.
 *
 * This file defines the AI strategy system that handles autonomous decision-making
 * for entities in the battle simulation. The strategy pattern allows for different
 * AI behaviors while maintaining a consistent interface for the simulation engine.
 *
 * Key responsibilities:
 * - Autonomous decision-making and behavior
 * - Target selection and prioritization
 * - Combat tactics and positioning
 * - Turn-based action planning and execution
 */

#pragma once

#include "../Types.hpp"

#include <memory>

namespace sw::core
{

	class Entity;
	class World;

	/**
	 * @brief Strategy interface for AI behaviors
	 *
	 * Defines the contract for all AI strategies in the simulation.
	 * AI strategies handle autonomous decision-making for entities, including
	 * target selection, movement planning, and combat tactics. The strategy
	 * pattern allows for different AI personalities and behaviors while
	 * maintaining consistent interaction with the simulation engine.
	 */
	class IAIStrategy
	{
	public:
		IAIStrategy() = default;
		virtual ~IAIStrategy() = default;
		IAIStrategy(const IAIStrategy&) = delete;
		auto operator=(const IAIStrategy&) -> IAIStrategy& = delete;
		IAIStrategy(IAIStrategy&&) = delete;
		auto operator=(IAIStrategy&&) -> IAIStrategy& = delete;

		/**
		 * @brief Update AI logic for an entity during a turn
		 * @param self The entity controlled by this AI
		 * @param world The world containing all entities and state
		 * @param turn Current turn number for decision context
		 * @return true if an action was taken, false otherwise
		 */
		virtual auto update(Entity& self, World& world, TurnNumber turn) -> bool = 0;
	};

	/**
	 * @brief Concrete strategy for Swordsman AI behavior
	 *
	 * Implements aggressive melee-focused AI behavior for swordsman units.
	 * Swordsman AI prioritizes direct engagement with enemies, using movement
	 * to close distance and melee attacks for combat. The AI is designed to
	 * be straightforward and aggressive, suitable for front-line combat units.
	 *
	 * Behavior:
	 * - Seeks out nearest enemies for direct engagement
	 * - Uses movement to close distance to targets
	 * - Prioritizes melee attacks when in range
	 * - Simple but effective combat tactics
	 */
	class SwordsmanAIStrategy final : public IAIStrategy
	{
	public:
		/**
		 * @brief Update swordsman AI logic for the current turn
		 * @param self The swordsman entity
		 * @param world The simulation world
		 * @param turn Current turn number
		 * @return true if an action was taken, false otherwise
		 */
		auto update(Entity& self, World& world, TurnNumber turn) -> bool override;
	};

	/**
	 * @brief Concrete strategy for Hunter AI behavior
	 *
	 * Implements tactical ranged combat AI behavior for hunter units.
	 * Hunter AI uses both melee and ranged attacks strategically, maintaining
	 * optimal positioning to maximize damage while minimizing risk. The AI
	 * is designed to be more sophisticated than swordsman AI, using tactical
	 * positioning and attack selection.
	 *
	 * Behavior:
	 * - Evaluates tactical positioning for ranged attacks
	 * - Uses movement to maintain optimal combat range
	 * - Selects between melee and ranged attacks based on situation
	 * - More complex decision-making for tactical advantage
	 */
	class HunterAIStrategy final : public IAIStrategy
	{
	public:
		/**
		 * @brief Update hunter AI logic for the current turn
		 * @param self The hunter entity
		 * @param world The simulation world
		 * @param turn Current turn number
		 * @return true if an action was taken, false otherwise
		 */
		auto update(Entity& self, World& world, TurnNumber turn) -> bool override;
	};

	/**
	 * @brief Factory function for creating swordsman AI strategies
	 * @return Unique pointer to swordsman AI strategy
	 */
	auto createSwordsmanAI() -> std::unique_ptr<IAIStrategy>;

	/**
	 * @brief Factory function for creating hunter AI strategies
	 * @return Unique pointer to hunter AI strategy
	 */
	auto createHunterAI() -> std::unique_ptr<IAIStrategy>;

}  // namespace sw::core
