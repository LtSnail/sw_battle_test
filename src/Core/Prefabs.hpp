/**
 * @file Prefabs.hpp
 * @brief Entity factory functions for creating predefined unit types.
 *
 * This file provides factory functions for creating common unit types with
 * predefined configurations of strategies and behaviors. The prefab system
 * uses the component-based architecture of the Entity class to compose
 * different unit types from reusable strategy components.
 *
 * Key responsibilities:
 * - Entity composition and configuration
 * - Strategy component assembly
 * - Unit type standardization
 * - Factory pattern implementation for unit creation
 */

#pragma once

#include "Core/Types.hpp"
#include "Entity.hpp"
#include "Strategies/AIStrategies.hpp"
#include "Strategies/AttackStrategies.hpp"
#include "Strategies/HealthStrategies.hpp"
#include "Strategies/MovementStrategies.hpp"

#include <memory>

namespace sw::core
{

	// Forward declarations
	class IHealthStrategy;
	class IMovementStrategy;
	class IAttackStrategy;
	class IAIStrategy;

	/**
	 * @brief Configuration struct for swordsman unit creation
	 */
	struct SwordsmanConfig
	{
		HealthPoints hp;
		StrengthValue strength;
	};

	/**
	 * @brief Configuration struct for hunter unit creation
	 */
	struct HunterConfig
	{
		HealthPoints hp;
		AgilityValue agility;
		StrengthValue strength;
		RangeValue range;
	};

	/**
	 * @brief Create a swordsman unit with predefined configuration
	 *
	 * Swordsmen are melee-focused units with moderate health and strength-based
	 * combat capabilities. They use basic AI for autonomous behavior and have
	 * standard terrain movement capabilities.
	 *
	 * Configuration:
	 * - Health: Basic health system with specified HP
	 * - Movement: 1-square terrain movement
	 * - Combat: Melee attack using strength attribute
	 * - AI: Swordsman-specific AI behavior
	 *
	 * @param id Unique identifier for the unit
	 * @param pos Initial position on the map
	 * @param config Swordsman configuration containing health and strength
	 * @return Unique pointer to the created swordsman entity
	 */
	inline auto makeSwordsman(UnitId id, Position pos, const SwordsmanConfig& config) -> std::unique_ptr<Entity>
	{
		auto entity = std::make_unique<Entity>(id, pos, "Swordsman");
		entity->setHealth(createBasicHealth(config.hp));
		entity->setMovement(createTerrainMovement(static_cast<RangeValue>(1)));
		entity->addAttack(createMeleeAttack(static_cast<DamageValue>(config.strength)));
		entity->setAI(sw::core::createSwordsmanAI());
		return entity;
	}

	/**
	 * @brief Create a hunter unit with predefined configuration
	 *
	 * Hunters are versatile units with both melee and ranged combat capabilities.
	 * They have moderate health, standard movement, and can engage enemies at
	 * both close and long range using different attack methods.
	 *
	 * Configuration:
	 * - Health: Basic health system with specified HP
	 * - Movement: 1-square terrain movement
	 * - Combat: Melee attack (strength-based) and ranged attack (agility-based)
	 *   - Ranged attack requires clear adjacency (no units in adjacent cells)
	 * - AI: Hunter-specific AI behavior for tactical combat
	 *
	 * @param id Unique identifier for the unit
	 * @param pos Initial position on the map
	 * @param config Hunter configuration containing hp, agility, strength, and range
	 * @return Unique pointer to the created hunter entity
	 */
	inline auto makeHunter(UnitId id, Position pos, const HunterConfig& config) -> std::unique_ptr<Entity>
	{
		auto entity = std::make_unique<Entity>(id, pos, "Hunter");
		entity->setHealth(createBasicHealth(config.hp));
		entity->setMovement(createTerrainMovement(static_cast<RangeValue>(1)));
		entity->addAttack(createMeleeAttack(static_cast<DamageValue>(config.strength)));
		entity->addAttack(createRangedAttack(
			static_cast<DamageValue>(config.agility), static_cast<RangeValue>(2), config.range, true));
		entity->setAI(sw::core::createHunterAI());
		return entity;
	}

}
