/**
 * @file Simulation.hpp
 * @brief Main simulation engine orchestrating the battle simulation lifecycle.
 *
 * The Simulation class is the primary interface for running battle simulations.
 * It coordinates the entire simulation lifecycle from setup through completion,
 * managing the world state, turn processing, and simulation events. The simulation
 * engine handles command processing, entity spawning, march target management,
 * and turn-based simulation execution.
 *
 * Key responsibilities:
 * - Simulation lifecycle management (start, run, end)
 * - Command processing and world setup
 * - Entity spawning and march target management
 * - Turn-based simulation execution
 * - Simulation state tracking and statistics
 */

#pragma once

#include "Core/Types.hpp"
#include "World.hpp"

#include <limits>
#include <optional>
#include <unordered_map>

namespace sw::core
{

	/**
	 * @brief Main simulation engine orchestrating the battle simulation lifecycle
	 *
	 * The Simulation class serves as the primary interface for the battle simulation
	 * system. It coordinates all aspects of the simulation from initial setup through
	 * completion, managing the world state, entity lifecycle, and turn-based execution.
	 *
	 * Architecture:
	 * - Manages the World instance containing all simulation data
	 * - Processes commands to set up the initial simulation state
	 * - Tracks march targets for autonomous movement behavior
	 * - Executes turn-based simulation with AI-driven entity behavior
	 * - Provides simulation statistics and state queries
	 *
	 * The simulation uses a turn-based execution model where each turn processes
	 * all entities in a deterministic order, allowing for AI-driven decisions
	 * and combat resolution.
	 */
	class Simulation
	{
	public:
		/**
		 * @brief Construct a new simulation instance
		 */
		Simulation();

		// === Setup and Configuration ===

		/**
		 * @brief Create the simulation map
		 * @param width Map width in grid units
		 * @param height Map height in grid units
		 * @return true if map creation was successful
		 */
		auto createMap(uint32_t width, uint32_t height) -> bool;

		/**
		 * @brief Spawn a swordsman unit
		 * @param unitId Unique identifier for the unit
		 * @param x Initial x coordinate
		 * @param y Initial y coordinate
		 * @param hp Health points
		 * @param strength Strength attribute for melee combat
		 * @return true if spawning was successful
		 */
		auto spawnSwordsman(UnitId unitId, uint32_t x, uint32_t y, HealthPoints hp, StrengthValue strength) -> bool;

		/**
		 * @brief Spawn a hunter unit
		 * @param unitId Unique identifier for the unit
		 * @param x Initial x coordinate
		 * @param y Initial y coordinate
		 * @param hp Health points
		 * @param agility Agility attribute for ranged combat
		 * @param strength Strength attribute for melee combat
		 * @param range Attack range for ranged weapons
		 * @return true if spawning was successful
		 */
		auto spawnHunter(
			UnitId unitId,
			uint32_t x,
			uint32_t y,
			HealthPoints hp,
			AgilityValue agility,
			StrengthValue strength,
			RangeValue range) -> bool;

		/**
		 * @brief Execute a march command (immediate execution)
		 * @param unitId Unit to march
		 * @param x Target x coordinate
		 * @param y Target y coordinate
		 * @return true if march command was successful
		 */
		struct MarchCommand
		{
			UnitId unitId;
			uint32_t x;
			uint32_t y;
		};

		auto executeMarch(const MarchCommand& command) -> bool;

		/**
		 * @brief Set a march target for a unit (deferred execution)
		 * @param command
		 * @return true if march target was set successfully
		 */
		auto setMarchTarget(const MarchCommand& command) -> bool;

		// === Simulation Execution ===

		/**
		 * @brief Run the battle simulation
		 * @param maxTurns Maximum number of turns to run (default: unlimited)
		 */
		void runSimulation(TurnNumber maxTurns = std::numeric_limits<TurnNumber>::max());

		// === State Queries ===

		/**
		 * @brief Get the number of active units in the simulation
		 * @return Number of units that are still alive
		 */
		[[nodiscard]]
		auto getActiveUnitCount() const -> size_t;

		/**
		 * @brief Check if a specific unit is active
		 * @param unitId Unit identifier to check
		 * @return true if unit exists and is alive
		 */
		[[nodiscard]]
		auto isUnitActive(UnitId unitId) const -> bool;

		/**
		 * @brief Get the position of a specific unit
		 * @param unitId Unit identifier
		 * @return Optional position, or nullopt if unit doesn't exist
		 */
		[[nodiscard]]
		auto getUnitPosition(UnitId unitId) const -> std::optional<Position>;

	private:
		World _world;										 ///< The simulation world containing all entities
		TurnNumber _currentTurn{1};							 ///< Current turn number
		std::unordered_map<UnitId, Position> _marchTargets;	 ///< Active march targets for autonomous movement

		/**
		 * @brief Check if the simulation should end
		 * @return true if simulation should end (1 or fewer active units)
		 */
		[[nodiscard]]
		auto shouldEndSimulation() const -> bool;

		/**
		 * @brief Process one turn of the simulation
		 * @return true if any actions were performed during the turn
		 */
		auto processTurn() -> bool;

		/**
		 * @brief Clean up march targets for inactive units
		 */
		void cleanupMarchTargets();
	};

}
