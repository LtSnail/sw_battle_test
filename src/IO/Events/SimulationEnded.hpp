#pragma once

#include <cstdint>

namespace sw::io
{
	struct SimulationEnded
	{
		constexpr static const char* Name = "SIMULATION_ENDED";

		uint32_t finalTurn{};
		uint32_t survivors{};
		uint32_t totalTurns{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("finalTurn", finalTurn);
			visitor.visit("survivors", survivors);
			visitor.visit("totalTurns", totalTurns);
		}
	};
}
