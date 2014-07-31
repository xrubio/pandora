
#include <CultureWorld.hxx>
#include <CultureConfig.hxx>
#include <Exception.hxx>
#include <Config.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{	
		if(argc>2)
		{
			throw Engine::Exception("USAGE: cultureDissemination [config file]");
		}		
	
		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}
		Examples::CultureWorld world(new Examples::CultureConfig(fileName), world.useOpenMPSingleNode());
	
		world.initialize(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

