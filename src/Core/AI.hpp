/**
 * @file AI.hpp
 * @brief AI utility functions and helper classes for autonomous entity behavior.
 *
 * This file provides utility functions and helper classes that support the
 * AI strategy system. It includes functions for enemy detection, target selection,
 * and random number generation that are used by various AI strategies to implement
 * autonomous decision-making behavior for entities.
 *
 * Key responsibilities:
 * - Enemy detection and gathering
 * - Target selection algorithms
 * - Random number generation for AI decisions
 * - Utility functions for AI strategy implementations
 */

#pragma once

#include <random>
#include <vector>

namespace sw::core
{

	// Forward declarations
	class Entity;
	class World;

	/**
	 * @brief AI utility functions and helper implementations
	 *
	 * This namespace contains utility functions that support AI strategy implementations.
	 * These functions provide common AI behaviors such as enemy detection, target selection,
	 * and randomization that can be used across different AI strategies.
	 */
	namespace detail
	{
		/**
		 * @brief Get the global random number engine for AI decisions
		 * @return Reference to the shared random number generator
		 */
		auto randomEngine() -> std::mt19937&;

		/**
		 * @brief Shuffle a vector of enemies to add randomness to target selection
		 * @param enemies Vector of enemy entities to shuffle
		 */
		void shuffleEnemies(std::vector<Entity*>& enemies);

		/**
		 * @brief Gather all enemy entities from the world
		 * @param self The entity performing the search
		 * @param world The world to search for enemies
		 * @return Vector of enemy entities
		 */
		auto gatherEnemies(const Entity& self, World& world) -> std::vector<Entity*>;

		/**
		 * @brief Find the nearest enemy from a list of enemies
		 * @param self The entity performing the search
		 * @param enemies Vector of enemy entities to search
		 * @return Pointer to the nearest enemy, or nullptr if no enemies
		 */
		auto findNearestEnemy(const Entity& self, const std::vector<Entity*>& enemies) -> Entity*;
	}

}
