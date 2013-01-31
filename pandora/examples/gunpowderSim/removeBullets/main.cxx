
#include "RemoveBulletWorld.hxx"
#include "Exceptions.hxx"

#include <iostream>
#include <cstdlib>

// this simulations tries to generate data of a XVIIIth century battlefield
// basic engagament, 0.5m each cell

int main(int argc, char *argv[])
{
	try
	{
		if(argc!=4)
		{
			throw Engine::Exception("USAGE: removeBullets [percentage removed=90] [battlefieldFile] [resultsPath]");
		}
		// TODO arguments readed from a file
		int percentage = atoi(argv[1]);
		std::string battlefieldFile(argv[2]);
		std::string resultsPath(argv[3]);

		BattleSim::RemoveBulletWorld degradedWorld(resultsPath, battlefieldFile, percentage);
		degradedWorld.init(argc, argv);
		degradedWorld.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

