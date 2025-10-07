#include "Entity.hpp"

#include "Core/Strategies/AIStrategies.hpp"
#include "Core/Strategies/AttackStrategies.hpp"
#include "Core/Strategies/HealthStrategies.hpp"
#include "Core/Strategies/MovementStrategies.hpp"
#include "Core/Types.hpp"

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace sw::core
{

	Entity::Entity(const UnitId id, const Position position, std::string typeName) :
			_id(id),
			_position(position),
			_typeName(std::move(typeName))
	{}

	void Entity::setHealth(std::unique_ptr<IHealthStrategy> health)
	{
		_health = std::move(health);
	}

	auto Entity::health() const -> std::optional<IHealthStrategy*>
	{
		if (_health.has_value())
		{
			return _health.value().get();
		}
		return std::nullopt;
	}

	void Entity::setMovement(std::unique_ptr<IMovementStrategy> movement)
	{
		_movement = std::move(movement);
	}

	auto Entity::movement() const -> std::optional<IMovementStrategy*>
	{
		if (_movement.has_value())
		{
			return _movement.value().get();
		}
		return std::nullopt;
	}

	void Entity::addAttack(std::unique_ptr<IAttackStrategy> attack)
	{
		_attacks.push_back(std::move(attack));
	}

	auto Entity::attacks() const -> const std::vector<std::unique_ptr<IAttackStrategy>>&
	{
		return _attacks;
	}

	void Entity::setAI(std::unique_ptr<IAIStrategy> ai)
	{
		_ai = std::move(ai);
	}

	auto Entity::ai() const -> std::optional<IAIStrategy*>
	{
		if (_ai.has_value())
		{
			return _ai.value().get();
		}
		return std::nullopt;
	}

	auto Entity::isAlive() const -> bool
	{
		if (const auto healthComponent = health())
		{
			return (*healthComponent)->isAlive();
		}
		return true;  // No health component = immortal
	}

	auto Entity::blocksGround() const -> bool
	{
		if (const auto movementStrategy = movement())
		{
			return (*movementStrategy)->blocksGround();
		}
		return true;  // Default: blocks ground if no movement strategy
	}

	auto Entity::canMove() const -> bool
	{
		return _movement.has_value();
	}

}
