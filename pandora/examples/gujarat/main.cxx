
#include <GujaratConfig.hxx>
#include <GujaratWorld.hxx>
#include <Simulation.hxx>
#include <Exceptions.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{
		if(argc>2)
		{
			throw Engine::Exception("USAGE: gujarat [config file]");
		}		

		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}
		Gujarat::GujaratConfig config;
		std::cout << "Loading config file: " << fileName << std::endl;
		config.deserialize(fileName);

		Engine::Simulation gujaratSim(config.getSize(), config.getNumSteps());
		Gujarat::GujaratWorld world( gujaratSim , config);
	
		world.init(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

