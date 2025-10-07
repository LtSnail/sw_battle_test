#include "Map.hpp"

#include "Core/Types.hpp"

#include <optional>

namespace sw::core
{

	Map::Map(const Dimensions& dimensions) :
			_width(dimensions.width),
			_height(dimensions.height)
	{}

	auto Map::placeUnit(const UnitId id, const Position pos, const bool blocksGround) -> bool
	{
		if (!isValidPosition(pos))
		{
			return false;
		}

		// Check if position is already occupied by another unit (single-unit-per-cell constraint)
		auto existingUnit = getUnitAt(pos);
		if (existingUnit.has_value())
		{
			return false;
		}

		if (blocksGround && blocksAt(pos))
		{
			return false;
		}

		_unitPositions[id] = pos;
		if (blocksGround)
		{
			_blockedPositions.insert(pos);
		}
		return true;
	}

	void Map::removeUnit(const UnitId id)
	{
		auto it = _unitPositions.find(id);
		if (it != _unitPositions.end())
		{
			_blockedPositions.erase(it->second);
			_unitPositions.erase(it);
		}
	}

	auto Map::moveUnit(const UnitId id, const Position newPos) -> bool
	{
		auto it = _unitPositions.find(id);
		if (it == _unitPositions.end())
		{
			return false;
		}

		if (!isValidPosition(newPos))
		{
			return false;
		}

		// Check if destination is occupied by another unit (single-unit-per-cell constraint)
		auto existingUnit = getUnitAt(newPos);
		if (existingUnit.has_value() && existingUnit.value() != id)
		{
			return false;
		}

		const Position oldPos = it->second;
		_blockedPositions.erase(oldPos);
		_unitPositions[id] = newPos;
		// Note: We don't automatically add to _blockedPositions here because
		// we don't have access to the unit's blocksGround() property in this context.
		// The caller (World::tryMove) should handle this properly.
		return true;
	}

	auto Map::blocksAt(Position pos) const noexcept -> bool
	{
		return _blockedPositions.contains(pos);
	}

	auto Map::isPositionOccupiedBy(const Position pos, const UnitId id) const noexcept -> bool
	{
		const auto it = _unitPositions.find(id);
		return it != _unitPositions.end() && it->second == pos;
	}

	auto Map::getUnitAt(const Position pos) const -> std::optional<UnitId>
	{
		for (const auto& [id, position] : _unitPositions)
		{
			if (position == pos)
			{
				return id;
			}
		}
		return std::nullopt;
	}

	void Map::setPositionBlocked(Position pos, bool blocked)
	{
		if (blocked)
		{
			_blockedPositions.insert(pos);
		}
		else
		{
			_blockedPositions.erase(pos);
		}
	}

}
