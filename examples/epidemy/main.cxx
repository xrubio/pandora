
#include <Earth.hxx>
#include <EarthConfig.hxx>
#include <Exception.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{	
		if(argc>2)
		{
			throw Engine::Exception("USAGE: epidemy [config file]");
		}		
	
		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}
		Examples::Earth earth( new Examples::EarthConfig(fileName), earth.useSpacePartition(15));
	
		earth.initialize(argc,argv);
		earth.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

