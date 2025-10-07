/**
 * @file Map.hpp
 * @brief Grid-based map system for spatial management and collision detection.
 *
 * The Map class represents the 2D grid world where entities exist and interact.
 * It manages spatial relationships, collision detection, and movement validation
 * for all entities in the simulation. The map uses a grid-based coordinate system
 * with support for unit placement, movement tracking, and ground blocking.
 *
 * Key responsibilities:
 * - Spatial coordinate validation
 * - Unit position tracking and management
 * - Collision detection and ground blocking
 * - Movement validation and pathfinding support
 */

#pragma once

#include "Types.hpp"

#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace sw::core
{

	/**
	 * @brief Grid-based map system for spatial management and collision detection
	 *
	 * The Map class provides the spatial foundation for the battle simulation.
	 * It maintains a 2D grid coordinate system where entities can be placed,
	 * moved, and interact. The map tracks unit positions and provides collision
	 * detection to prevent overlapping placement and enable pathfinding.
	 *
	 * Features:
	 * - Grid-based coordinate system with configurable dimensions
	 * - Unit position tracking with fast lookup capabilities
	 * - Ground blocking system for collision detection
	 * - Spatial validation and boundary checking
	 *
	 * The map uses efficient data structures (unordered_map, unordered_set) for
	 * fast spatial queries and collision detection during simulation updates.
	 */
	class Map
	{
	public:
		/**
		 * @brief Default constructor for empty map
		 */
		Map() = default;

		/**
		 * @brief Map dimensions configuration
		 */
		struct Dimensions
		{
			uint32_t width;
			uint32_t height;
		};

		/**
		 * @brief Construct a map with specified dimensions
		 * @param dimensions Map dimensions configuration
		 */
		explicit Map(const Dimensions& dimensions);

		// === Unit Management ===

		/**
		 * @brief Place a unit at the specified position
		 * @param id Unit identifier
		 * @param pos Position to place the unit
		 * @param blocksGround Whether the unit blocks ground movement
		 * @return true if placement was successful, false if position is invalid or occupied
		 */
		auto placeUnit(UnitId id, Position pos, bool blocksGround) -> bool;

		/**
		 * @brief Remove a unit from the map
		 * @param id Unit identifier to remove
		 */
		void removeUnit(UnitId id);

		/**
		 * @brief Move a unit to a new position
		 * @param id Unit identifier
		 * @param newPos New position for the unit
		 * @return true if movement was successful, false if position is invalid or occupied
		 */
		auto moveUnit(UnitId id, Position newPos) -> bool;

		// === Spatial Queries ===

		/**
		 * @brief Check if a position is within map boundaries
		 * @param pos Position to validate
		 * @return true if position is within map bounds
		 */
		[[nodiscard]]
		constexpr auto isValidPosition(Position pos) const noexcept -> bool
		{
			return pos.isWithin(_width, _height);
		}

		/**
		 * @brief Check if a position is blocked by a unit
		 * @param pos Position to check
		 * @return true if position is blocked by a ground-blocking unit
		 */
		[[nodiscard]]
		auto blocksAt(Position pos) const noexcept -> bool;

		/**
		 * @brief Check if a position is occupied by a specific unit
		 * @param pos Position to check
		 * @param id Unit identifier to check for
		 * @return true if the specified unit occupies the position
		 */
		[[nodiscard]]
		auto isPositionOccupiedBy(Position pos, UnitId id) const noexcept -> bool;

		/**
		 * @brief Get the unit at a specific position
		 * @param pos Position to query
		 * @return Optional unit ID if a unit exists at the position, nullopt otherwise
		 */
		[[nodiscard]]
		auto getUnitAt(Position pos) const -> std::optional<UnitId>;

		/**
		 * @brief Set whether a position is blocked for ground movement
		 * @param pos Position to set blocking status for
		 * @param blocked Whether the position should be blocked
		 */
		void setPositionBlocked(Position pos, bool blocked);

	private:
		uint32_t _width{0};									  ///< Map width in grid units
		uint32_t _height{0};								  ///< Map height in grid units
		std::unordered_map<UnitId, Position> _unitPositions;  ///< Mapping from unit ID to position
		std::unordered_set<Position> _blockedPositions;		  ///< Set of positions blocked by units
	};

}
