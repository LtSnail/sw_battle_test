#include "Simulation.hpp"

#include "AI.hpp"
#include "Core/Types.hpp"
#include "IO/Events/MarchEnded.hpp"
#include "IO/Events/MarchStarted.hpp"
#include "IO/Events/SimulationEnded.hpp"
#include "IO/Events/SimulationStarted.hpp"
#include "IO/System/EventLog.hpp"
#include "Prefabs.hpp"

#include <memory>
#include <optional>
#include <ranges>
#include <utility>

namespace sw::core
{

	Simulation::Simulation() = default;

	auto Simulation::createMap(const uint32_t width, const uint32_t height) -> bool
	{
		_world.reset(width, height, std::make_unique<sw::EventLog>());
		_marchTargets.clear();
		_currentTurn = 1;
		return true;
	}

	auto Simulation::spawnSwordsman(
		const UnitId unitId, const uint32_t x, const uint32_t y, const HealthPoints hp, const StrengthValue strength)
		-> bool
	{
		if (_world.getEntity(unitId) != nullptr)
		{
			return false;
		}

		auto entity = makeSwordsman(unitId, Position{.x = x, .y = y}, SwordsmanConfig{.hp = hp, .strength = strength});
		_world.addEntity(std::move(entity));
		return true;
	}

	auto Simulation::spawnHunter(
		const UnitId unitId,
		const uint32_t x,
		const uint32_t y,
		const HealthPoints hp,
		const AgilityValue agility,
		const StrengthValue strength,
		const RangeValue range) -> bool
	{
		if (_world.getEntity(unitId) != nullptr)
		{
			return false;
		}

		auto entity = makeHunter(
			unitId,
			Position{.x = x, .y = y},
			HunterConfig{.hp = hp, .agility = agility, .strength = strength, .range = range});
		_world.addEntity(std::move(entity));
		return true;
	}

	auto Simulation::executeMarch(const MarchCommand& command) -> bool
	{
		auto* entity = _world.getEntity(command.unitId);
		if (entity == nullptr)
		{
			return false;
		}

		const Position target{.x = command.x, .y = command.y};
		if (!_world.map().isValidPosition(target))
		{
			return false;
		}

		_marchTargets[command.unitId] = target;

		sw::io::MarchStarted event;
		event.unitId = command.unitId;
		event.x = entity->position().x;
		event.y = entity->position().y;
		event.targetX = target.x;
		event.targetY = target.y;
		_world.eventLog().log(_currentTurn, event);

		return true;
	}

	auto Simulation::setMarchTarget(const MarchCommand& command) -> bool
	{
		auto* entity = _world.getEntity(command.unitId);
		if (entity == nullptr)
		{
			return false;
		}

		const Position target{.x = command.x, .y = command.y};
		if (!_world.map().isValidPosition(target))
		{
			return false;
		}

		_marchTargets[command.unitId] = target;

		sw::io::MarchStarted event;
		event.unitId = command.unitId;
		event.x = entity->position().x;
		event.y = entity->position().y;
		event.targetX = target.x;
		event.targetY = target.y;
		_world.eventLog().log(1, event);  // Log at turn 1 during setup

		return true;
	}

	void Simulation::runSimulation(TurnNumber maxTurns)
	{
		// Log simulation start
		sw::io::SimulationStarted startEvent;
		startEvent.unitCount = getActiveUnitCount();
		startEvent.turn = _currentTurn;
		_world.eventLog().log(_currentTurn, startEvent);

		TurnNumber startTurn = _currentTurn;

		for (; _currentTurn <= maxTurns; ++_currentTurn)
		{
			if (shouldEndSimulation())
			{
				break;
			}

			bool actionPerformed = processTurn();
			cleanupMarchTargets();
			_world.flushPendingRemovals();

			if (!actionPerformed)
			{
				break;
			}
		}

		// Log simulation end
		sw::io::SimulationEnded endEvent;
		endEvent.finalTurn = _currentTurn;
		endEvent.survivors = getActiveUnitCount();
		endEvent.totalTurns = _currentTurn - startTurn;
		_world.eventLog().log(_currentTurn, endEvent);
	}

	auto Simulation::getActiveUnitCount() const -> size_t
	{
		size_t count = 0;
		for (const auto& entity : _world.entities() | std::views::values)
		{
			if (entity && entity->isAlive())
			{
				++count;
			}
		}
		return count;
	}

	auto Simulation::isUnitActive(UnitId unitId) const -> bool
	{
		const auto* entity = _world.getEntity(unitId);
		return entity != nullptr && entity->isAlive();
	}

	auto Simulation::getUnitPosition(UnitId unitId) const -> std::optional<Position>
	{
		const auto* entity = _world.getEntity(unitId);
		if (entity == nullptr)
		{
			return std::nullopt;
		}
		return entity->position();
	}

	auto Simulation::shouldEndSimulation() const -> bool
	{
		return getActiveUnitCount() <= 1;
	}

	auto Simulation::processTurn() -> bool
	{
		bool anyAction = false;
		auto order = _world.entityOrder();
		for (UnitId id : order)
		{
			auto* entity = _world.getEntity(id);
			if (entity == nullptr || !entity->isAlive())
			{
				continue;
			}

			bool marched = false;
			auto marchIt = _marchTargets.find(id);
			if (marchIt != _marchTargets.end())
			{
				marched = _world.moveEntityTowards(*entity, marchIt->second, _currentTurn);
				if (marched)
				{
					anyAction = true;
				}
				if (entity->position() == marchIt->second)
				{
					sw::io::MarchEnded event;
					event.unitId = id;
					event.x = entity->position().x;
					event.y = entity->position().y;
					_world.eventLog().log(_currentTurn, event);
					_marchTargets.erase(marchIt);
					anyAction = true;
				}
			}

			if (!marched)
			{
				if (auto ai = entity->ai(); ai && entity->isAlive())
				{
					if ((*ai)->update(*entity, _world, _currentTurn))
					{
						anyAction = true;
					}
				}
			}
		}

		return anyAction;
	}

	void Simulation::cleanupMarchTargets()
	{
		for (auto it = _marchTargets.begin(); it != _marchTargets.end();)
		{
			if (const auto* entity = _world.getEntity(it->first); entity == nullptr || !entity->isAlive())
			{
				it = _marchTargets.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

}
