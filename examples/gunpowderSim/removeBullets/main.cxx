
#include "RemoveBulletWorld.hxx"
#include "RemoveBulletConfig.hxx"
#include <Exception.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{	
        if(argc>2)
		{
			throw Engine::Exception("USAGE: removeBullets [config file]");
		}
		
		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}
		BattleSim::RemoveBulletWorld degradedWorld( new BattleSim::RemoveBulletConfig(fileName), degradedWorld.useOpenMPSingleNode());
		
        degradedWorld.initialize(argc, argv);
		degradedWorld.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

