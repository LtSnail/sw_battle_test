#include "MovementStrategies.hpp"

#include "../Entity.hpp"
#include "../World.hpp"
#include "Core/Types.hpp"

#include <algorithm>
#include <memory>
#include <utility>

namespace sw::core
{
	namespace
	{
		auto stepTowards(const Position from, const Position target, const RangeValue step) -> Position
		{
			if (from == target)
			{
				return from;
			}

			int dx = static_cast<int>(target.x) - static_cast<int>(from.x);
			int dy = static_cast<int>(target.y) - static_cast<int>(from.y);

			if (dx != 0)
			{
				dx = (dx > 0) ? 1 : -1;
			}
			if (dy != 0)
			{
				dy = (dy > 0) ? 1 : -1;
			}

			int nx = static_cast<int>(from.x);
			int ny = static_cast<int>(from.y);
			for (RangeValue i = 0; i < step; ++i)
			{
				if (std::cmp_equal(nx, target.x) && std::cmp_equal(ny, target.y))
				{
					break;
				}

				nx += dx;
				ny += dy;
			}

			nx = std::max(nx, 0);
			ny = std::max(ny, 0);

			return Position{.x = static_cast<uint32_t>(nx), .y = static_cast<uint32_t>(ny)};
		}
	}

	TerrainMovementStrategy::TerrainMovementStrategy(RangeValue step) :
			_step(step)
	{}

	auto TerrainMovementStrategy::move(Entity& self, World& world, Position target, TurnNumber turn) -> bool
	{
		Position current = self.position();
		if (current == target || _step == 0U)
		{
			return false;
		}

		// Check if target is within movement range
		RangeValue distance = current.distanceTo(target);
		if (distance > _step)
		{
			// Target is too far, move towards it step by step
			Position desired = stepTowards(current, target, _step);
			return world.tryMove(self, desired, turn, false);
		}

		// Target is within range, move directly to it
		return world.tryMove(self, target, turn, false);
	}

	// Factory function implementations
	auto createTerrainMovement(RangeValue step) -> std::unique_ptr<IMovementStrategy>
	{
		return std::make_unique<TerrainMovementStrategy>(step);
	}

}  // namespace sw::core
