
#include <NeolithicWorld.hxx>
#include <NeolithicConfig.hxx>
#include <Exception.hxx>
#include <Simulation.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{
		if(argc>2)
		{
			throw Engine::Exception("USAGE: neolithic [config file]");
		}
		
		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}
		Examples::NeolithicConfig config;
		config.deserialize(fileName);
	
		Engine::Simulation neoSim(config.getSize(), config.getNumSteps(), config.getSerializeResolution());
		Examples::NeolithicWorld world( config, neoSim, world.useOpenMPSingleNode(config.getResultsFile()));
	
		world.initialize(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

