
#include <Earth.hxx>
#include <EarthConfig.hxx>
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
			throw Engine::Exception("USAGE: zombieAttack [config file]");
		}		
	
		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}
		Examples::EarthConfig config;
		config.deserialize(fileName);
	
		Engine::Simulation zombieAttackSim(config.getSize(), config.getNumSteps());
		Examples::Earth earth( zombieAttackSim, config);
	
		earth.init(argc, argv);
		earth.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

