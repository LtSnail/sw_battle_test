#include <Core/Simulation.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/System/CommandParser.hpp>
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
	using namespace sw;

	if (argc != 2)
	{
		std::cerr << "Usage:" << '\n';
		std::cerr << "  " << argv[0] << " <scenario_file>  - Run simulation with scenario file" << '\n';
		return 1;
	}

	std::ifstream file(argv[1]);
	if (!file)
	{
		throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
	}

	core::Simulation simulation;

	bool mapCreated = false;

	io::CommandParser parser;
	parser.add<io::CreateMap>(
		[&simulation, &mapCreated](io::CreateMap command)
		{
			if (!simulation.createMap(command.width, command.height))
			{
				throw std::runtime_error("Failed to create map");
			}
			mapCreated = true;
		});

	parser.add<io::SpawnSwordsman>(
		[&simulation](const io::SpawnSwordsman& command)
		{
			if (!simulation.spawnSwordsman(command.unitId, command.x, command.y, command.hp, command.strength))
			{
				throw std::runtime_error(
					"Failed to spawn swordsman at position (" + std::to_string(command.x) + ","
					+ std::to_string(command.y) + ")");
			}
		});

	parser.add<io::SpawnHunter>(
		[&simulation](const io::SpawnHunter& command)
		{
			if (!simulation.spawnHunter(
					command.unitId, command.x, command.y, command.hp, command.agility, command.strength, command.range))
			{
				throw std::runtime_error(
					"Failed to spawn hunter at position (" + std::to_string(command.x) + "," + std::to_string(command.y)
					+ ")");
			}
		});

	parser.add<io::March>(
		[&simulation](const io::March command)
		{
			if (!simulation.setMarchTarget({.unitId=command.unitId, .x=command.targetX, .y=command.targetY}))
			{
				throw std::runtime_error(
				"Failed to set march target for unit " + std::to_string(command.unitId) + " to position ("
				+ std::to_string(command.targetX) + "," + std::to_string(command.targetY) + "). "
				"Position may be out of bounds.");
			}
		});

	// Parse commands and execute them
	parser.parse(file);

	// Run the battle simulation
	if (mapCreated && simulation.getActiveUnitCount() > 1)
	{
		simulation.runSimulation();
	}

	return 0;
}
