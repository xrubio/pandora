
#include <Scenario.hxx>
#include <ScenarioConfig.hxx>
#include <Exceptions.hxx>
#include <Simulation.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{	
		if(argc>2)
		{
			throw Engine::Exception("USAGE: panic [config file]");
		}		
	
		std::string fileName("config.xml");
		if(argc==2)
		{
			fileName = argv[1];
		}
		Panic::ScenarioConfig config;
		config.deserialize(fileName);
	
		Engine::Simulation panicSimulation(config.getSize(), config.getNumSteps());
		Panic::Scenario scenario( panicSimulation, config);
	
		scenario.init(argc, argv);
		scenario.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

