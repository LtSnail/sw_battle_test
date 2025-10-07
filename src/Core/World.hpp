/**
 * @file World.hpp
 * @brief Central game world management system coordinating all simulation components.
 *
 * The World class serves as the central coordinator for the entire battle simulation.
 * It manages the spatial map, entity lifecycle, event logging, and high-level game
 * mechanics. The World acts as the bridge between the simulation engine and the
 * individual components, providing a unified interface for entity management,
 * spatial operations, and event tracking.
 *
 * Key responsibilities:
 * - Entity lifecycle management (creation, removal, updates)
 * - Spatial coordination between entities and map
 * - Event logging and tracking
 * - High-level game mechanics (movement, combat, AI coordination)
 * - Turn order management and simulation state
 */

#pragma once

#include "Core/Types.hpp"
#include "Entity.hpp"
#include "IO/System/EventLog.hpp"
#include "Map.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace sw::core
{

	/**
	 * @brief Central game world management system coordinating all simulation components
	 *
	 * The World class is the heart of the battle simulation system. It coordinates
	 * all major subsystems including the spatial map, entity management, event logging,
	 * and game mechanics. The World provides a unified interface for the simulation
	 * engine to interact with entities, manage spatial relationships, and track events.
	 *
	 * Architecture:
	 * - Manages the spatial Map for collision detection and movement validation
	 * - Maintains entity collection with efficient lookup capabilities
	 * - Coordinates entity lifecycle (creation, updates, removal)
	 * - Provides high-level game mechanics (movement, combat, AI)
	 * - Integrates with EventLog for comprehensive event tracking
	 * - Manages turn order for deterministic simulation updates
	 *
	 * The World uses deferred removal to ensure safe iteration during simulation
	 * updates, preventing iterator invalidation during entity removal.
	 */
	class World
	{
	public:
		/**
		 * @brief Default constructor for empty world
		 */
		World();

		/**
		 * @brief Construct a world with specified map dimensions and event logging
		 * @param width Map width in grid units
		 * @param height Map height in grid units
		 * @param log Event log for tracking simulation events
		 */
		World(uint32_t width, uint32_t height, std::unique_ptr<sw::EventLog> log);

		/**
		 * @brief Reset the world with new dimensions and event log
		 * @param width New map width in grid units
		 * @param height New map height in grid units
		 * @param log New event log for tracking simulation events
		 */
		void reset(uint32_t width, uint32_t height, std::unique_ptr<sw::EventLog> log);

		// === Core System Access ===

		/**
		 * @brief Get the spatial map system
		 * @return Reference to the map for spatial operations
		 */
		auto map() -> Map&
		{
			return _map;
		}

		/**
		 * @brief Get the spatial map system (const)
		 * @return Const reference to the map for spatial operations
		 */
		[[nodiscard]]
		auto map() const -> const Map&
		{
			return _map;
		}

		/**
		 * @brief Get the event logging system
		 * @return Reference to the event log for tracking events
		 */
		[[nodiscard]]
		auto eventLog() const -> sw::EventLog&
		{
			return *_eventLog;
		}

		/**
		 * @brief Get the entity turn order (const)
		 * @return Const reference to the entity order vector
		 */
		[[nodiscard]]
		auto entityOrder() const -> const std::vector<UnitId>&
		{
			return _entityOrder;
		}

		/**
		 * @brief Get the entity turn order for modification
		 * @return Reference to the entity order vector
		 */
		auto entityOrder() -> std::vector<UnitId>&
		{
			return _entityOrder;
		}

		// === Entity Management ===

		/**
		 * @brief Get an entity by its ID
		 * @param id Unit identifier
		 * @return Pointer to entity, or nullptr if not found
		 */
		auto getEntity(UnitId id) -> Entity*;

		/**
		 * @brief Get an entity by its ID (const)
		 * @param id Unit identifier
		 * @return Const pointer to entity, or nullptr if not found
		 */
		[[nodiscard]]
		auto getEntity(UnitId id) const -> const Entity*;

		/**
		 * @brief Add an entity to the world
		 * @param entity Unique pointer to the entity to add
		 * @return Reference to the added entity
		 */
		auto addEntity(std::unique_ptr<Entity> entity) -> Entity&;

		/**
		 * @brief Remove an entity from the world (deferred removal)
		 * @param id Unit identifier to remove
		 */
		void removeEntity(UnitId id);

		/**
		 * @brief Process all pending entity removals
		 *
		 * Called at the end of each turn to safely remove entities
		 * that were marked for removal during the turn.
		 */
		void flushPendingRemovals();

		// === Game Mechanics ===

		/**
		 * @brief Attempt to move an entity to a destination
		 * @param entity Entity to move
		 * @param destination Target position
		 * @param turn Current turn number
		 * @param ignoreBlocking Whether to ignore blocking entities
		 * @return true if movement was successful
		 */
		auto tryMove(Entity& entity, Position destination, TurnNumber turn, bool ignoreBlocking) -> bool;

		/**
		 * @brief Damage application configuration
		 */
		struct DamageConfig
		{
			int damage;
			TurnNumber turn;
		};

		/**
		 * @brief Apply damage to a target entity
		 * @param attacker Entity dealing damage
		 * @param target Entity receiving damage
		 * @param config Damage configuration containing amount and turn
		 */
		void applyDamage(const Entity& attacker, const Entity& target, const DamageConfig& config);

		// === High-Level AI Actions ===

		/**
		 * @brief Move an entity towards a target position (with pathfinding)
		 * @param entity Entity to move
		 * @param target Target position
		 * @param turn Current turn number
		 * @return true if entity moved closer to target
		 */
		auto moveEntityTowards(Entity& entity, Position target, TurnNumber turn) -> bool;

		/**
		 * @brief Execute an attack from attacker to target
		 * @param attacker Entity performing the attack
		 * @param target Entity being attacked
		 * @param turn Current turn number
		 * @param preferred Optional preferred attack type
		 * @return true if attack was executed successfully
		 */
		auto executeAttack(
			Entity& attacker, Entity& target, TurnNumber turn, std::optional<AttackType> preferred = std::nullopt)
			-> bool;

		// === Entity Collection Access ===

		/**
		 * @brief Get the entity collection for iteration
		 * @return Reference to the entity map
		 */
		auto entities() -> std::unordered_map<UnitId, std::unique_ptr<Entity>>&
		{
			return _entities;
		}

		/**
		 * @brief Get the entity collection for iteration (const)
		 * @return Const reference to the entity map
		 */
		[[nodiscard]]
		auto entities() const -> const std::unordered_map<UnitId, std::unique_ptr<Entity>>&
		{
			return _entities;
		}

	private:
		Map _map;  ///< Spatial map for collision detection and movement
		std::unordered_map<UnitId, std::unique_ptr<Entity>> _entities;	///< Entity collection with fast ID lookup
		std::vector<UnitId> _entityOrder;								///< Turn order for deterministic simulation
		std::unique_ptr<sw::EventLog> _eventLog;						///< Event logging system
		std::unordered_set<UnitId> _pendingRemoval;						///< Entities marked for deferred removal

		/**
		 * @brief Log map creation event
		 * @param dimensions Map dimensions
		 */
		void logMapCreated(const Map::Dimensions& dimensions) const;

		/**
		 * @brief Schedule an entity for removal at end of turn
		 * @param id Unit identifier to mark for removal
		 */
		void scheduleRemoval(UnitId id);
	};

}
