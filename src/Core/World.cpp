#include "World.hpp"

#include "Core/Entity.hpp"
#include "Core/Types.hpp"
#include "IO/Events/MapCreated.hpp"
#include "IO/Events/UnitAttacked.hpp"
#include "IO/Events/UnitDied.hpp"
#include "IO/Events/UnitMoved.hpp"
#include "IO/Events/UnitSpawned.hpp"
#include "IO/System/EventLog.hpp"

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace sw::core
{

	World::World() :
			_map({.width = 0, .height = 0}),
			_eventLog(std::make_unique<EventLog>())
	{}

	World::World(uint32_t width, uint32_t height, std::unique_ptr<EventLog> log)
	{
		reset(width, height, std::move(log));
	}

	void World::reset(const uint32_t width, const uint32_t height, std::unique_ptr<EventLog> log)
	{
		_map = Map({.width = width, .height = height});
		_entities.clear();
		_entityOrder.clear();
		_eventLog = std::move(log);
		_pendingRemoval.clear();
		if (!_eventLog)
		{
			_eventLog = std::make_unique<EventLog>();
		}

		logMapCreated({.width = width, .height = height});
	}

	auto World::getEntity(const UnitId id) -> Entity*
	{
		const auto it = _entities.find(id);
		return (it != _entities.end()) ? it->second.get() : nullptr;
	}

	auto World::getEntity(const UnitId id) const -> const Entity*
	{
		const auto it = _entities.find(id);
		return (it != _entities.end()) ? it->second.get() : nullptr;
	}

	auto World::addEntity(std::unique_ptr<Entity> entity) -> Entity&
	{
		if (!entity)
		{
			throw std::invalid_argument("entity must not be null");
		}

		UnitId id = entity->id();
		const Position pos = entity->position();

		if (!_map.placeUnit(id, pos, entity->blocksGround()))
		{
			throw std::runtime_error("failed to place entity on map");
		}

		const std::string typeName = entity->typeName();
		Entity& stored = *_entities.emplace(id, std::move(entity)).first->second;
		_entityOrder.push_back(id);

		io::UnitSpawned event;
		event.unitId = id;
		event.unitType = typeName;
		event.x = pos.x;
		event.y = pos.y;
		eventLog().log(1, event);

		return stored;
	}

	void World::removeEntity(UnitId id)
	{
		_map.removeUnit(id);
		_entities.erase(id);
		std::erase(_entityOrder, id);
		_pendingRemoval.erase(id);
	}

	auto World::tryMove(Entity& entity, Position destination, TurnNumber turn, bool ignoreBlocking) -> bool
	{
		if (!entity.isAlive())
		{
			return false;
		}

		if (const Position current = entity.position(); current == destination)
		{
			return false;
		}

		if (!_map.isValidPosition(destination))
		{
			return false;
		}

		if (!ignoreBlocking && _map.blocksAt(destination) && !_map.isPositionOccupiedBy(destination, entity.id()))
		{
			return false;
		}

		if (!_map.moveUnit(entity.id(), destination))
		{
			return false;
		}

		// Update blocked positions based on entity's blocksGround property
		if (entity.blocksGround())
		{
			_map.setPositionBlocked(destination, true);
		}

		entity.setPosition(destination);

		io::UnitMoved event;
		event.unitId = entity.id();
		event.x = destination.x;
		event.y = destination.y;
		eventLog().log(turn, event);

		return true;
	}

	void World::applyDamage(const Entity& attacker, const Entity& target, const DamageConfig& config)
	{
		if (config.damage <= 0)
		{
			return;
		}

		const auto health = target.health();
		if (!health)
		{
			return;
		}

		if (!(*health)->isAlive())
		{
			return;
		}

		(*health)->applyDamage(config.damage);

		io::UnitAttacked event;
		event.attackerUnitId = attacker.id();
		event.targetUnitId = target.id();
		event.damage = config.damage;
		event.targetHp = (*health)->hitPoints();
		eventLog().log(config.turn, event);

		if (!(*health)->isAlive())
		{
			sw::io::UnitDied diedEvent;
			diedEvent.unitId = target.id();
			eventLog().log(config.turn, diedEvent);
			scheduleRemoval(target.id());
		}
	}

	auto World::moveEntityTowards(Entity& entity, Position target, TurnNumber turn) -> bool
	{
		const auto movement = entity.movement();
		if (!movement)
		{
			return false;
		}
		return (*movement)->move(entity, *this, target, turn);
	}

	auto World::executeAttack(Entity& attacker, Entity& target, TurnNumber turn, std::optional<AttackType> preferred)
		-> bool
	{
		auto attempt = [&](AttackType filter) -> bool
		{
			for (const auto& attack : attacker.attacks())
			{
				if (attack->type() != filter)
				{
					continue;
				}
				if (attack->attack(attacker, target, *this, turn))
				{
					return true;
				}
			}
			return false;
		};

		if (preferred.has_value())
		{
			if (attempt(*preferred))
			{
				return true;
			}
		}

		for (const auto& attack : attacker.attacks())
		{
			if (attack->attack(attacker, target, *this, turn))
			{
				return true;
			}
		}

		return false;
	}

	void World::logMapCreated(const Map::Dimensions& dimensions) const
	{
		io::MapCreated event;
		event.width = dimensions.width;
		event.height = dimensions.height;
		eventLog().log(1, event);
	}

	void World::scheduleRemoval(UnitId id)
	{
		_pendingRemoval.insert(id);
	}

	void World::flushPendingRemovals()
	{
		if (_pendingRemoval.empty())
		{
			return;
		}

		const std::vector toRemove(_pendingRemoval.begin(), _pendingRemoval.end());
		_pendingRemoval.clear();
		for (const UnitId id : toRemove)
		{
			removeEntity(id);
		}
	}

}
