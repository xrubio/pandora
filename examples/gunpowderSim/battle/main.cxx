
#include "Battlefield.hxx"
#include <Exception.hxx>

#include <iostream>
#include <cstdlib>

#include "BattlefieldConfig.hxx"

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
		BattleSim::Battlefield battle(new BattleSim::BattlefieldConfig(fileName), battle.useOpenMPSingleNode());
		
		battle.initialize(argc, argv);
		battle.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

