#pragma once

#include <cstdint>

namespace sw::io
{
	struct SimulationStarted
	{
		constexpr static const char* Name = "SIMULATION_STARTED";

		uint32_t unitCount{};
		uint32_t turn{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitCount", unitCount);
			visitor.visit("turn", turn);
		}
	};
}
