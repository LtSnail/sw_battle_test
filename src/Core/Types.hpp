/**
 * @file Types.hpp
 * @brief Core type definitions and fundamental data structures for the battle simulation system.
 *
 * This file contains all the fundamental types used throughout the simulation system,
 * including unit identifiers, combat attributes, spatial coordinates, and attack mechanics.
 * It serves as the foundation for the entire battle simulation architecture.
 */

#pragma once

#include <algorithm>  // For std::max
#include <cstddef>
#include <cstdint>	// For uint32_t, std::uint8_t
#include <cstdlib>
#include <functional>

namespace sw::core
{

	/**
	 * @brief Core type aliases for battle simulation attributes
	 *
	 * These type aliases provide semantic meaning to primitive types and ensure
	 * consistent usage across the codebase. They represent the fundamental
	 * attributes that define unit capabilities and characteristics.
	 */
	using UnitId = uint32_t;		 ///< Unique identifier for each unit in the simulation
	using HealthPoints = uint32_t;	 ///< Health/HP value representing unit vitality
	using StrengthValue = uint32_t;	 ///< Strength attribute for melee combat damage
	using AgilityValue = uint32_t;	 ///< Agility attribute for ranged combat and movement
	using RangeValue = uint32_t;	 ///< Attack range for ranged weapons
	using DamageValue = uint32_t;	 ///< Damage amount for attacks and combat
	using TurnNumber = uint32_t;	 ///< Turn number for simulation tracking

	/**
	 * @brief Damage calculation types for different combat mechanics
	 *
	 * Determines which unit attribute is used to calculate damage output.
	 * This allows for different combat systems (melee vs ranged) with
	 * distinct damage calculation formulas.
	 */
	enum class DamageType : std::uint8_t
	{
		Strength,  ///< Melee damage based on unit's strength attribute
		Agility	   ///< Ranged damage based on unit's agility attribute
	};

	/**
	 * @brief Attack method types for combat system
	 *
	 * Defines the method of attack, which affects range, damage calculation,
	 * and other combat mechanics. Used by the attack strategy system to
	 * determine appropriate combat behavior.
	 */
	enum class AttackType : std::uint8_t
	{
		Melee,	///< Close-range combat attack
		Ranged	///< Long-range combat attack
	};

	/**
	 * @brief Grid-based coordinate system for spatial positioning
	 *
	 * Represents a position on the 2D grid map used by the simulation.
	 * Provides essential spatial operations including distance calculation,
	 * boundary checking, and comparison operations for pathfinding and combat.
	 *
	 * The coordinate system uses zero-based indexing with (0,0) in the top-left corner.
	 */
	struct Position
	{
		uint32_t x = 0;	 ///< Horizontal coordinate (column)
		uint32_t y = 0;	 ///< Vertical coordinate (row)

		/**
		 * @brief Equality comparison for positions
		 */
		auto operator==(const Position& other) const noexcept -> bool = default;

		/**
		 * @brief Lexicographical ordering for use in ordered containers
		 *
		 * Provides consistent ordering: (x1,y1) < (x2,y2) if x1 < x2 or (x1 == x2 and y1 < y2)
		 */
		constexpr auto operator<(const Position& other) const noexcept -> bool
		{
			return x < other.x || (x == other.x && y < other.y);
		}

		/**
		 * @brief Calculate Chebyshev distance between two positions
		 *
		 * Uses 8-directional movement (king's move in chess) which allows
		 * diagonal movement. This is appropriate for grid-based pathfinding
		 * and combat range calculations.
		 *
		 * @param other Target position
		 * @return Distance in grid units
		 */
		[[nodiscard]]
		constexpr auto distanceTo(const Position& other) const noexcept -> uint32_t
		{
			return std::max(
				std::abs(static_cast<int32_t>(x) - static_cast<int32_t>(other.x)),
				std::abs(static_cast<int32_t>(y) - static_cast<int32_t>(other.y)));
		}

		/**
		 * @brief Check if position is within map boundaries
		 *
		 * @param mapWidth Width of the map (exclusive boundary)
		 * @param mapHeight Height of the map (exclusive boundary)
		 * @return true if position is valid within the map bounds
		 */
		[[nodiscard]]
		constexpr auto isWithin(uint32_t mapWidth, uint32_t mapHeight) const noexcept -> bool
		{
			return x < mapWidth && y < mapHeight;
		}
	};

}

/**
 * @brief Hash specialization for Position to enable use in unordered containers
 *
 * Provides a hash function for Position objects, allowing them to be used as keys
 * in std::unordered_map and std::unordered_set. Uses a simple XOR-based hash
 * combining the x and y coordinates.
 */
template <>
struct std::hash<sw::core::Position>
{
	/**
	 * @brief Compute hash value for a Position
	 * @param pos Position to hash
	 * @return Hash value suitable for unordered containers
	 */
	constexpr auto operator()(const sw::core::Position& pos) const noexcept -> size_t
	{
		return std::hash<uint32_t>{}(pos.x) ^ (std::hash<uint32_t>{}(pos.y) << 1);
	}
};
