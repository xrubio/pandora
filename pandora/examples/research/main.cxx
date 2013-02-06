
#include <Academia.hxx>
#include <AcademiaConfig.hxx>
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
			throw Engine::Exception("USAGE: research [config file]");
		}		
	
		std::string fileName("config.xml");
		if(argc==2)
		{
			fileName = argv[1];
		}
		Examples::AcademiaConfig config;
		config.deserialize(fileName);
	
		Engine::Simulation academiaSimulation(config.getSize(), config.getNumSteps());
		Examples::Academia academia( academiaSimulation, config);
	
		academia.init(argc, argv);
		academia.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

