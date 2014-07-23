
#include "NeighborConfig.hxx"
#include "Neighborhood.hxx"
#include <Exception.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{
		if(argc>=2)
		{
			throw Engine::Exception("USAGE: segregation [config file]");
		}		

		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}
		Segregation::Neighborhood neighborhood( new Segregation::NeighborConfig(fileName), neighborhood.useOpenMPSingleNode());
		neighborhood.initialize(argc, argv);
		neighborhood.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

