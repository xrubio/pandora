
#include "Battlefield.hxx"
#include "Exceptions.hxx"

#include <iostream>
#include <cstdlib>

#include "Battlefield.hxx"
#include "BattlefieldConfig.hxx"
#include "Simulation.hxx"

// this simulations tries to generate data of a XVIIIth century battlefield
// basic engagament, 0.5m each cell

int main(int argc, char *argv[])
{
	try
	{
		if(argc>2)
		{
			throw Engine::Exception("USAGE: battleSim [config file]");
		}
		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}
		BattleSim::BattlefieldConfig config;		
		config.deserialize(fileName);

		Engine::Simulation sim(config._size, config.getNumSteps());
		BattleSim::Battlefield battle(sim,config);
		
		battle.init(argc, argv);
		battle.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

